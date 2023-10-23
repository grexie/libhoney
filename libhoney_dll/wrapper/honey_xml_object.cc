// Copyright (c) 2010 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_xml_object.h"

#include <sstream>

#include "include/base/honey_logging.h"
#include "include/honey_stream.h"

namespace {

class HoneycombXmlObjectLoader {
 public:
  explicit HoneycombXmlObjectLoader(HoneycombRefPtr<HoneycombXmlObject> root_object)
      : root_object_(root_object) {}

  HoneycombXmlObjectLoader(const HoneycombXmlObjectLoader&) = delete;
  HoneycombXmlObjectLoader& operator=(const HoneycombXmlObjectLoader&) = delete;

  bool Load(HoneycombRefPtr<HoneycombStreamReader> stream,
            HoneycombXmlReader::EncodingType encodingType,
            const HoneycombString& URI) {
    HoneycombRefPtr<HoneycombXmlReader> reader(
        HoneycombXmlReader::Create(stream, encodingType, URI));
    if (!reader.get()) {
      return false;
    }

    bool ret = reader->MoveToNextNode();
    if (ret) {
      HoneycombRefPtr<HoneycombXmlObject> cur_object(root_object_), new_object;
      HoneycombXmlObject::ObjectVector queue;
      int cur_depth, value_depth = -1;
      HoneycombXmlReader::NodeType cur_type;
      std::stringstream cur_value;
      bool last_has_ns = false;

      queue.push_back(root_object_);

      do {
        cur_depth = reader->GetDepth();
        if (value_depth >= 0 && cur_depth > value_depth) {
          // The current node has already been parsed as part of a value.
          continue;
        }

        cur_type = reader->GetType();
        if (cur_type == XML_NODE_ELEMENT_START) {
          if (cur_depth == value_depth) {
            // Add to the current value.
            cur_value << std::string(reader->GetOuterXml());
            continue;
          } else if (last_has_ns && reader->GetPrefix().empty()) {
            if (!cur_object->HasChildren()) {
              // Start a new value because the last element has a namespace and
              // this element does not.
              value_depth = cur_depth;
              cur_value << std::string(reader->GetOuterXml());
            } else {
              // Value following a child element is not allowed.
              std::stringstream ss;
              ss << "Value following child element, line "
                 << reader->GetLineNumber();
              load_error_ = ss.str();
              ret = false;
              break;
            }
          } else {
            // Start a new element.
            new_object = new HoneycombXmlObject(reader->GetQualifiedName());
            cur_object->AddChild(new_object);
            last_has_ns = !reader->GetPrefix().empty();

            if (!reader->IsEmptyElement()) {
              // The new element potentially has a value and/or children, so
              // set the current object and add the object to the queue.
              cur_object = new_object;
              queue.push_back(cur_object);
            }

            if (reader->HasAttributes() && reader->MoveToFirstAttribute()) {
              // Read all object attributes.
              do {
                new_object->SetAttributeValue(reader->GetQualifiedName(),
                                              reader->GetValue());
              } while (reader->MoveToNextAttribute());
              reader->MoveToCarryingElement();
            }
          }
        } else if (cur_type == XML_NODE_ELEMENT_END) {
          if (cur_depth == value_depth) {
            // Ending an element that is already in the value.
            continue;
          } else if (cur_depth < value_depth) {
            // Done with parsing the value portion of the current element.
            cur_object->SetValue(cur_value.str());
            cur_value.str("");
            value_depth = -1;
          }

          // Pop the current element from the queue.
          queue.pop_back();

          if (queue.empty() ||
              cur_object->GetName() != reader->GetQualifiedName()) {
            // Open tag without close tag or close tag without open tag should
            // never occur (the parser catches this error).
            DCHECK(false);
            std::stringstream ss;
            ss << "Mismatched end tag for "
               << std::string(cur_object->GetName()) << ", line "
               << reader->GetLineNumber();
            load_error_ = ss.str();
            ret = false;
            break;
          }

          // Set the current object to the previous object in the queue.
          cur_object = queue.back().get();
        } else if (cur_type == XML_NODE_TEXT || cur_type == XML_NODE_CDATA ||
                   cur_type == XML_NODE_ENTITY_REFERENCE) {
          if (cur_depth == value_depth) {
            // Add to the current value.
            cur_value << std::string(reader->GetValue());
          } else if (!cur_object->HasChildren()) {
            // Start a new value.
            value_depth = cur_depth;
            cur_value << std::string(reader->GetValue());
          } else {
            // Value following a child element is not allowed.
            std::stringstream ss;
            ss << "Value following child element, line "
               << reader->GetLineNumber();
            load_error_ = ss.str();
            ret = false;
            break;
          }
        }
      } while (reader->MoveToNextNode());
    }

    if (reader->HasError()) {
      load_error_ = reader->GetError();
      return false;
    }

    return ret;
  }

  HoneycombString GetLoadError() { return load_error_; }

 private:
  HoneycombString load_error_;
  HoneycombRefPtr<HoneycombXmlObject> root_object_;
};

}  // namespace

HoneycombXmlObject::HoneycombXmlObject(const HoneycombString& name)
    : name_(name), parent_(nullptr) {}

HoneycombXmlObject::~HoneycombXmlObject() {}

bool HoneycombXmlObject::Load(HoneycombRefPtr<HoneycombStreamReader> stream,
                        HoneycombXmlReader::EncodingType encodingType,
                        const HoneycombString& URI,
                        HoneycombString* loadError) {
  Clear();

  HoneycombXmlObjectLoader loader(this);
  if (!loader.Load(stream, encodingType, URI)) {
    if (loadError) {
      *loadError = loader.GetLoadError();
    }
    return false;
  }
  return true;
}

void HoneycombXmlObject::Set(HoneycombRefPtr<HoneycombXmlObject> object) {
  DCHECK(object.get());

  Clear();

  name_ = object->GetName();
  Append(object, true);
}

void HoneycombXmlObject::Append(HoneycombRefPtr<HoneycombXmlObject> object,
                          bool overwriteAttributes) {
  DCHECK(object.get());

  if (object->HasChildren()) {
    ObjectVector children;
    object->GetChildren(children);
    ObjectVector::const_iterator it = children.begin();
    for (; it != children.end(); ++it) {
      AddChild((*it)->Duplicate());
    }
  }

  if (object->HasAttributes()) {
    AttributeMap attributes;
    object->GetAttributes(attributes);
    AttributeMap::const_iterator it = attributes.begin();
    for (; it != attributes.end(); ++it) {
      if (overwriteAttributes || !HasAttribute(it->first)) {
        SetAttributeValue(it->first, it->second);
      }
    }
  }
}

HoneycombRefPtr<HoneycombXmlObject> HoneycombXmlObject::Duplicate() {
  HoneycombRefPtr<HoneycombXmlObject> new_obj;
  {
    base::AutoLock lock_scope(lock_);
    new_obj = new HoneycombXmlObject(name_);
    new_obj->Append(this, true);
  }
  return new_obj;
}

void HoneycombXmlObject::Clear() {
  ClearChildren();
  ClearAttributes();
}

HoneycombString HoneycombXmlObject::GetName() {
  HoneycombString name;
  {
    base::AutoLock lock_scope(lock_);
    name = name_;
  }
  return name;
}

bool HoneycombXmlObject::SetName(const HoneycombString& name) {
  DCHECK(!name.empty());
  if (name.empty()) {
    return false;
  }

  base::AutoLock lock_scope(lock_);
  name_ = name;
  return true;
}

bool HoneycombXmlObject::HasParent() {
  base::AutoLock lock_scope(lock_);
  return (parent_ != nullptr);
}

HoneycombRefPtr<HoneycombXmlObject> HoneycombXmlObject::GetParent() {
  HoneycombRefPtr<HoneycombXmlObject> parent;
  {
    base::AutoLock lock_scope(lock_);
    parent = parent_;
  }
  return parent;
}

bool HoneycombXmlObject::HasValue() {
  base::AutoLock lock_scope(lock_);
  return !value_.empty();
}

HoneycombString HoneycombXmlObject::GetValue() {
  HoneycombString value;
  {
    base::AutoLock lock_scope(lock_);
    value = value_;
  }
  return value;
}

bool HoneycombXmlObject::SetValue(const HoneycombString& value) {
  base::AutoLock lock_scope(lock_);
  DCHECK(children_.empty());
  if (!children_.empty()) {
    return false;
  }
  value_ = value;
  return true;
}

bool HoneycombXmlObject::HasAttributes() {
  base::AutoLock lock_scope(lock_);
  return !attributes_.empty();
}

size_t HoneycombXmlObject::GetAttributeCount() {
  base::AutoLock lock_scope(lock_);
  return attributes_.size();
}

bool HoneycombXmlObject::HasAttribute(const HoneycombString& name) {
  if (name.empty()) {
    return false;
  }

  base::AutoLock lock_scope(lock_);
  AttributeMap::const_iterator it = attributes_.find(name);
  return (it != attributes_.end());
}

HoneycombString HoneycombXmlObject::GetAttributeValue(const HoneycombString& name) {
  DCHECK(!name.empty());
  HoneycombString value;
  if (!name.empty()) {
    base::AutoLock lock_scope(lock_);
    AttributeMap::const_iterator it = attributes_.find(name);
    if (it != attributes_.end()) {
      value = it->second;
    }
  }
  return value;
}

bool HoneycombXmlObject::SetAttributeValue(const HoneycombString& name,
                                     const HoneycombString& value) {
  DCHECK(!name.empty());
  if (name.empty()) {
    return false;
  }

  base::AutoLock lock_scope(lock_);
  AttributeMap::iterator it = attributes_.find(name);
  if (it != attributes_.end()) {
    it->second = value;
  } else {
    attributes_.insert(std::make_pair(name, value));
  }
  return true;
}

size_t HoneycombXmlObject::GetAttributes(AttributeMap& attributes) {
  base::AutoLock lock_scope(lock_);
  attributes = attributes_;
  return attributes_.size();
}

void HoneycombXmlObject::ClearAttributes() {
  base::AutoLock lock_scope(lock_);
  attributes_.clear();
}

bool HoneycombXmlObject::HasChildren() {
  base::AutoLock lock_scope(lock_);
  return !children_.empty();
}

size_t HoneycombXmlObject::GetChildCount() {
  base::AutoLock lock_scope(lock_);
  return children_.size();
}

bool HoneycombXmlObject::HasChild(HoneycombRefPtr<HoneycombXmlObject> child) {
  DCHECK(child.get());

  base::AutoLock lock_scope(lock_);
  ObjectVector::const_iterator it = children_.begin();
  for (; it != children_.end(); ++it) {
    if ((*it).get() == child.get()) {
      return true;
    }
  }
  return false;
}

bool HoneycombXmlObject::AddChild(HoneycombRefPtr<HoneycombXmlObject> child) {
  DCHECK(child.get());
  if (!child.get()) {
    return false;
  }

  HoneycombRefPtr<HoneycombXmlObject> parent = child->GetParent();
  DCHECK(!parent);
  if (parent) {
    return false;
  }

  base::AutoLock lock_scope(lock_);

  children_.push_back(child);
  child->SetParent(this);
  return true;
}

bool HoneycombXmlObject::RemoveChild(HoneycombRefPtr<HoneycombXmlObject> child) {
  DCHECK(child.get());

  base::AutoLock lock_scope(lock_);
  ObjectVector::iterator it = children_.begin();
  for (; it != children_.end(); ++it) {
    if ((*it).get() == child.get()) {
      children_.erase(it);
      child->SetParent(nullptr);
      return true;
    }
  }
  return false;
}

size_t HoneycombXmlObject::GetChildren(ObjectVector& children) {
  base::AutoLock lock_scope(lock_);
  children = children_;
  return children_.size();
}

void HoneycombXmlObject::ClearChildren() {
  base::AutoLock lock_scope(lock_);
  ObjectVector::iterator it = children_.begin();
  for (; it != children_.end(); ++it) {
    (*it)->SetParent(nullptr);
  }
  children_.clear();
}

HoneycombRefPtr<HoneycombXmlObject> HoneycombXmlObject::FindChild(const HoneycombString& name) {
  DCHECK(!name.empty());
  if (name.empty()) {
    return nullptr;
  }

  base::AutoLock lock_scope(lock_);
  ObjectVector::const_iterator it = children_.begin();
  for (; it != children_.end(); ++it) {
    if ((*it)->GetName() == name) {
      return (*it);
    }
  }
  return nullptr;
}

size_t HoneycombXmlObject::FindChildren(const HoneycombString& name,
                                  ObjectVector& children) {
  DCHECK(!name.empty());
  if (name.empty()) {
    return 0;
  }

  size_t ct = 0;

  base::AutoLock lock_scope(lock_);
  ObjectVector::const_iterator it = children_.begin();
  for (; it != children_.end(); ++it) {
    if ((*it)->GetName() == name) {
      children.push_back(*it);
      ct++;
    }
  }
  return ct;
}

void HoneycombXmlObject::SetParent(HoneycombXmlObject* parent) {
  base::AutoLock lock_scope(lock_);
  if (parent) {
    DCHECK(parent_ == nullptr);
    parent_ = parent;
  } else {
    DCHECK(parent_ != nullptr);
    parent_ = nullptr;
  }
}
