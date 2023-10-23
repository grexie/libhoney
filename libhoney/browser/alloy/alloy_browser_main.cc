// Copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/alloy/alloy_browser_main.h"

#include <stdint.h>

#include <string>

#include "libhoney/browser/alloy/dialogs/alloy_constrained_window_views_client.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/browser_context_keyed_service_factories.h"
#include "libhoney/browser/context.h"
#include "libhoney/browser/devtools/devtools_manager_delegate.h"
#include "libhoney/browser/extensions/extension_system_factory.h"
#include "libhoney/browser/file_dialog_runner.h"
#include "libhoney/browser/net/chrome_scheme_handler.h"
#include "libhoney/browser/permission_prompt.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/common/app_manager.h"
#include "libhoney/common/extensions/extensions_util.h"
#include "libhoney/common/net/net_resource_provider.h"

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/thread_pool.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/media/router/chrome_media_router_factory.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/ui/color/chrome_color_mixers.h"
#include "chrome/browser/ui/javascript_dialogs/chrome_javascript_app_modal_dialog_view_factory.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "components/constrained_window/constrained_window_views.h"
#include "content/public/browser/gpu_data_manager.h"
#include "content/public/browser/network_service_instance.h"
#include "content/public/common/result_codes.h"
#include "extensions/browser/extensions_browser_client.h"
#include "extensions/common/constants.h"
#include "net/base/net_module.h"
#include "third_party/widevine/cdm/buildflags.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/color/color_provider_manager.h"
#include "ui/native_theme/native_theme.h"

#if BUILDFLAG(IS_LINUX)
#include "ui/ozone/buildflags.h"
#if defined(USE_AURA) && BUILDFLAG(OZONE_PLATFORM_X11)
#include "ui/events/devices/x11/touch_factory_x11.h"
#endif
#endif

#if defined(USE_AURA)
#include "ui/aura/env.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#include "ui/wm/core/wm_state.h"

#if BUILDFLAG(IS_WIN)
#include "base/enterprise_util.h"
#include "base/files/file_util.h"
#include "chrome/browser/chrome_browser_main_win.h"
#include "chrome/browser/win/parental_controls.h"
#endif
#endif  // defined(USE_AURA)

#if BUILDFLAG(IS_MAC)
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/chrome_views_delegate.h"
#else
#include "ui/views/test/desktop_test_views_delegate.h"
#endif

#if defined(USE_AURA) && BUILDFLAG(IS_LINUX)
#include "ui/base/ime/init/input_method_initializer.h"
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)
#include "components/os_crypt/sync/os_crypt.h"
#endif

#if BUILDFLAG(IS_LINUX)
#include "base/path_service.h"
#include "chrome/browser/themes/theme_service_aura_linux.h"
#include "chrome/browser/ui/views/theme_profile_key.h"
#include "chrome/grit/chromium_strings.h"
#include "components/os_crypt/sync/key_storage_config_linux.h"
#include "libhoney/browser/printing/print_dialog_linux.h"
#include "ui/base/cursor/cursor_factory.h"
#include "ui/base/ime/input_method.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/linux/linux_ui.h"
#include "ui/linux/linux_ui_delegate.h"
#include "ui/linux/linux_ui_factory.h"
#include "ui/linux/linux_ui_getter.h"
#include "ui/ozone/public/ozone_platform.h"
#endif  // BUILDFLAG(IS_LINUX)

#if BUILDFLAG(ENABLE_MEDIA_FOUNDATION_WIDEVINE_CDM)
#include "chrome/browser/component_updater/media_foundation_widevine_cdm_component_installer.h"
#endif

#if BUILDFLAG(ENABLE_WIDEVINE_CDM_COMPONENT)
#include "chrome/browser/component_updater/widevine_cdm_component_installer.h"
#endif

namespace {

#if BUILDFLAG(IS_LINUX)

class LinuxUiGetterImpl : public ui::LinuxUiGetter {
 public:
  LinuxUiGetterImpl() = default;
  ~LinuxUiGetterImpl() override = default;
  ui::LinuxUiTheme* GetForWindow(aura::Window* window) override {
    return window ? GetForProfile(GetThemeProfileForWindow(window)) : nullptr;
  }
  ui::LinuxUiTheme* GetForProfile(Profile* profile) override {
    return ui::GetLinuxUiTheme(
        ThemeServiceAuraLinux::GetSystemThemeForProfile(profile));
  }
};

ui::LinuxUi* GetLinuxUI() {
  // We can't use GtkUi in combination with multi-threaded-message-loop because
  // Chromium's GTK implementation doesn't use GDK threads.
  if (!!HoneycombContext::Get()->settings().multi_threaded_message_loop) {
    return nullptr;
  }

  // If the ozone backend hasn't provided a LinuxUiDelegate, don't try to create
  // a LinuxUi instance as this may result in a crash in toolkit initialization.
  if (!ui::LinuxUiDelegate::GetInstance()) {
    return nullptr;
  }

  return ui::GetDefaultLinuxUi();
}

#endif  // BUILDFLAG(IS_LINUX)

}  // namespace

AlloyBrowserMainParts::AlloyBrowserMainParts() = default;

AlloyBrowserMainParts::~AlloyBrowserMainParts() {
  constrained_window::SetConstrainedWindowViewsClient(nullptr);
}

void AlloyBrowserMainParts::ToolkitInitialized() {
  SetConstrainedWindowViewsClient(CreateAlloyConstrainedWindowViewsClient());
#if defined(USE_AURA)
  CHECK(aura::Env::GetInstance());

  wm_state_.reset(new wm::WMState);
#endif  // defined(USE_AURA)

#if BUILDFLAG(IS_MAC)
  views_delegate_ = std::make_unique<ChromeViewsDelegate>();
  layout_provider_ = ChromeLayoutProvider::CreateLayoutProvider();
#else
  views_delegate_ = std::make_unique<views::DesktopTestViewsDelegate>();
#endif

#if BUILDFLAG(IS_LINUX)
  // Based on chrome_browser_main_extra_parts_views_linux.cc
  if (auto linux_ui = GetLinuxUI()) {
    linux_ui_getter_ = std::make_unique<LinuxUiGetterImpl>();
    ui::LinuxUi::SetInstance(linux_ui);

    // Cursor theme changes are tracked by LinuxUI (via a CursorThemeManager
    // implementation). Start observing them once it's initialized.
    ui::CursorFactory::GetInstance()->ObserveThemeChanges();
  }

  auto printing_delegate = new HoneycombPrintingContextLinuxDelegate();
  auto default_delegate =
      ui::PrintingContextLinuxDelegate::SetInstance(printing_delegate);
  printing_delegate->SetDefaultDelegate(default_delegate);
#endif  // BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_MAC)
  if (base::FeatureList::IsEnabled(features::kViewsJSAppModalDialog)) {
    InstallChromeJavaScriptAppModalDialogViewFactory();
  } else {
    InstallChromeJavaScriptAppModalDialogViewCocoaFactory();
  }
#else
  InstallChromeJavaScriptAppModalDialogViewFactory();
#endif

  // On GTK that builds the native theme that, in turn, adds the GTK core color
  // mixer; core mixers should all be added before we add chrome mixers.
  ui::ColorProviderManager::Get().AppendColorProviderInitializer(
      base::BindRepeating(AddChromeColorMixers));
}

void AlloyBrowserMainParts::PreCreateMainMessageLoop() {
#if BUILDFLAG(IS_LINUX)
#if defined(USE_AURA) && BUILDFLAG(OZONE_PLATFORM_X11)
  ui::TouchFactory::SetTouchDeviceListFromCommandLine();
#endif
#endif

#if BUILDFLAG(IS_WIN)
  // Initialize the OSCrypt.
  PrefService* local_state = g_browser_process->local_state();
  DCHECK(local_state);
  bool os_crypt_init = OSCrypt::Init(local_state);
  DCHECK(os_crypt_init);

  // installer_util references strings that are normally compiled into
  // setup.exe.  In Chrome, these strings are in the locale files.
  ChromeBrowserMainPartsWin::SetupInstallerUtilStrings();
#endif  // BUILDFLAG(IS_WIN)

  media_router::ChromeMediaRouterFactory::DoPlatformInit();
}

void AlloyBrowserMainParts::PostCreateMainMessageLoop() {
#if BUILDFLAG(IS_LINUX)
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  // Set up crypt config. This needs to be done before anything starts the
  // network service, as the raw encryption key needs to be shared with the
  // network service for encrypted cookie storage.
  // Based on ChromeBrowserMainPartsLinux::PostCreateMainMessageLoop.
  std::unique_ptr<os_crypt::Config> config =
      std::make_unique<os_crypt::Config>();
  // Forward to os_crypt the flag to use a specific password store.
  config->store = command_line->GetSwitchValueASCII(switches::kPasswordStore);
  // Forward the product name (defaults to "Chromium").
  config->product_name = l10n_util::GetStringUTF8(IDS_PRODUCT_NAME);
  // OSCrypt can be disabled in a special settings file.
  config->should_use_preference =
      command_line->HasSwitch(switches::kEnableEncryptionSelection);
  base::PathService::Get(chrome::DIR_USER_DATA, &config->user_data_path);
  DCHECK(!config->user_data_path.empty());
  OSCrypt::SetConfig(std::move(config));
#endif  // BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_WIN)
  base::SetExtraNoExecuteAllowedPath(chrome::DIR_USER_DATA);
#endif
}

int AlloyBrowserMainParts::PreCreateThreads() {
#if BUILDFLAG(IS_WIN)
  PlatformInitialize();
#endif

  net::NetModule::SetResourceProvider(&NetResourceProvider);

  // Initialize these objects before IO access restrictions are applied and
  // before the IO thread is started.
  content::GpuDataManager::GetInstance();
  SystemNetworkContextManager::CreateInstance(g_browser_process->local_state());

  return 0;
}

int AlloyBrowserMainParts::PreMainMessageLoopRun() {
#if defined(USE_AURA)
  screen_ = views::CreateDesktopScreen();
#endif
#if BUILDFLAG(IS_MAC)
  screen_ = std::make_unique<display::ScopedNativeScreen>();
#endif

  if (extensions::ExtensionsEnabled()) {
    // This should be set in ChromeBrowserProcessAlloy::Initialize.
    DCHECK(extensions::ExtensionsBrowserClient::Get());
    // Initialize extension global objects before creating the global
    // BrowserContext.
    extensions::HoneycombExtensionSystemFactory::GetInstance();
  }

  // Register additional KeyedService factories here. See
  // ChromeBrowserMainExtraPartsProfiles for details.
  honey::EnsureBrowserContextKeyedServiceFactoriesBuilt();

  background_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::BEST_EFFORT,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_visible_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_VISIBLE,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_blocking_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_BLOCKING,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});

  HoneycombRequestContextSettings settings;
  HoneycombContext::Get()->PopulateGlobalRequestContextSettings(&settings);

  // Create the global RequestContext.
  global_request_context_ =
      HoneycombRequestContextImpl::CreateGlobalRequestContext(settings);
  auto browser_context =
      global_request_context_->GetBrowserContext()->AsBrowserContext();

  HoneycombDevToolsManagerDelegate::StartHttpHandler(browser_context);

#if BUILDFLAG(IS_WIN)
  // Windows parental controls calls can be slow, so we do an early init here
  // that calculates this value off of the UI thread.
  InitializeWinParentalControls();

  // These methods may call LoadLibrary and could trigger
  // AssertBlockingAllowed() failures if executed at a later time on the UI
  // thread.
  base::IsManagedDevice();
  base::IsEnterpriseDevice();
#endif  // BUILDFLAG(IS_WIN)

  scheme::RegisterWebUIControllerFactory();
  file_dialog_runner::RegisterFactory();
  permission_prompt::RegisterCreateCallback();

  // Initialize theme configuration (high contrast, dark mode, etc).
  ui::NativeTheme::GetInstanceForNativeUi();

#if BUILDFLAG(ENABLE_MEDIA_FOUNDATION_WIDEVINE_CDM) || \
    BUILDFLAG(ENABLE_WIDEVINE_CDM_COMPONENT)
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (!command_line->HasSwitch(switches::kDisableComponentUpdate)) {
    auto* const cus = g_browser_process->component_updater();

#if BUILDFLAG(ENABLE_MEDIA_FOUNDATION_WIDEVINE_CDM)
    RegisterMediaFoundationWidevineCdmComponent(cus);
#endif

#if BUILDFLAG(ENABLE_WIDEVINE_CDM_COMPONENT)
    RegisterWidevineCdmComponent(cus);
#endif
  }
#endif

  return content::RESULT_CODE_NORMAL_EXIT;
}

void AlloyBrowserMainParts::PostMainMessageLoopRun() {
  // NOTE: Destroy objects in reverse order of creation.
  HoneycombDevToolsManagerDelegate::StopHttpHandler();

  // There should be no additional references to the global HoneycombRequestContext
  // during shutdown. Did you forget to release a HoneycombBrowser reference?
  DCHECK(global_request_context_->HasOneRef());
  global_request_context_ = nullptr;
}

void AlloyBrowserMainParts::PostDestroyThreads() {
  views_delegate_.reset();
#if BUILDFLAG(IS_MAC)
  layout_provider_.reset();
#endif
}
