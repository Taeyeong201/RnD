// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: echo.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_echo_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_echo_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3013000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3013000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_echo_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_echo_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_echo_2eproto;
namespace simple {
class EchoMessage;
class EchoMessageDefaultTypeInternal;
extern EchoMessageDefaultTypeInternal _EchoMessage_default_instance_;
class Message;
class MessageDefaultTypeInternal;
extern MessageDefaultTypeInternal _Message_default_instance_;
}  // namespace simple
PROTOBUF_NAMESPACE_OPEN
template<> ::simple::EchoMessage* Arena::CreateMaybeMessage<::simple::EchoMessage>(Arena*);
template<> ::simple::Message* Arena::CreateMaybeMessage<::simple::Message>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace simple {

// ===================================================================

class Message PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:simple.Message) */ {
 public:
  inline Message() : Message(nullptr) {}
  virtual ~Message();

  Message(const Message& from);
  Message(Message&& from) noexcept
    : Message() {
    *this = ::std::move(from);
  }

  inline Message& operator=(const Message& from) {
    CopyFrom(from);
    return *this;
  }
  inline Message& operator=(Message&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Message& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Message* internal_default_instance() {
    return reinterpret_cast<const Message*>(
               &_Message_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Message& a, Message& b) {
    a.Swap(&b);
  }
  inline void Swap(Message* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Message* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Message* New() const final {
    return CreateMaybeMessage<Message>(nullptr);
  }

  Message* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Message>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Message& from);
  void MergeFrom(const Message& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Message* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "simple.Message";
  }
  protected:
  explicit Message(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_echo_2eproto);
    return ::descriptor_table_echo_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTextFieldNumber = 1,
    kSizeFieldNumber = 2,
  };
  // string text = 1;
  void clear_text();
  const std::string& text() const;
  void set_text(const std::string& value);
  void set_text(std::string&& value);
  void set_text(const char* value);
  void set_text(const char* value, size_t size);
  std::string* mutable_text();
  std::string* release_text();
  void set_allocated_text(std::string* text);
  private:
  const std::string& _internal_text() const;
  void _internal_set_text(const std::string& value);
  std::string* _internal_mutable_text();
  public:

  // uint64 size = 2;
  void clear_size();
  ::PROTOBUF_NAMESPACE_ID::uint64 size() const;
  void set_size(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_size() const;
  void _internal_set_size(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // @@protoc_insertion_point(class_scope:simple.Message)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr text_;
  ::PROTOBUF_NAMESPACE_ID::uint64 size_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_echo_2eproto;
};
// -------------------------------------------------------------------

class EchoMessage PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:simple.EchoMessage) */ {
 public:
  inline EchoMessage() : EchoMessage(nullptr) {}
  virtual ~EchoMessage();

  EchoMessage(const EchoMessage& from);
  EchoMessage(EchoMessage&& from) noexcept
    : EchoMessage() {
    *this = ::std::move(from);
  }

  inline EchoMessage& operator=(const EchoMessage& from) {
    CopyFrom(from);
    return *this;
  }
  inline EchoMessage& operator=(EchoMessage&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const EchoMessage& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const EchoMessage* internal_default_instance() {
    return reinterpret_cast<const EchoMessage*>(
               &_EchoMessage_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(EchoMessage& a, EchoMessage& b) {
    a.Swap(&b);
  }
  inline void Swap(EchoMessage* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(EchoMessage* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline EchoMessage* New() const final {
    return CreateMaybeMessage<EchoMessage>(nullptr);
  }

  EchoMessage* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<EchoMessage>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const EchoMessage& from);
  void MergeFrom(const EchoMessage& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(EchoMessage* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "simple.EchoMessage";
  }
  protected:
  explicit EchoMessage(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_echo_2eproto);
    return ::descriptor_table_echo_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kEchoFieldNumber = 1,
    kSizeFieldNumber = 2,
  };
  // string echo = 1;
  void clear_echo();
  const std::string& echo() const;
  void set_echo(const std::string& value);
  void set_echo(std::string&& value);
  void set_echo(const char* value);
  void set_echo(const char* value, size_t size);
  std::string* mutable_echo();
  std::string* release_echo();
  void set_allocated_echo(std::string* echo);
  private:
  const std::string& _internal_echo() const;
  void _internal_set_echo(const std::string& value);
  std::string* _internal_mutable_echo();
  public:

  // uint64 size = 2;
  void clear_size();
  ::PROTOBUF_NAMESPACE_ID::uint64 size() const;
  void set_size(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_size() const;
  void _internal_set_size(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // @@protoc_insertion_point(class_scope:simple.EchoMessage)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr echo_;
  ::PROTOBUF_NAMESPACE_ID::uint64 size_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_echo_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Message

// string text = 1;
inline void Message::clear_text() {
  text_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& Message::text() const {
  // @@protoc_insertion_point(field_get:simple.Message.text)
  return _internal_text();
}
inline void Message::set_text(const std::string& value) {
  _internal_set_text(value);
  // @@protoc_insertion_point(field_set:simple.Message.text)
}
inline std::string* Message::mutable_text() {
  // @@protoc_insertion_point(field_mutable:simple.Message.text)
  return _internal_mutable_text();
}
inline const std::string& Message::_internal_text() const {
  return text_.Get();
}
inline void Message::_internal_set_text(const std::string& value) {
  
  text_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void Message::set_text(std::string&& value) {
  
  text_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:simple.Message.text)
}
inline void Message::set_text(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  text_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:simple.Message.text)
}
inline void Message::set_text(const char* value,
    size_t size) {
  
  text_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:simple.Message.text)
}
inline std::string* Message::_internal_mutable_text() {
  
  return text_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* Message::release_text() {
  // @@protoc_insertion_point(field_release:simple.Message.text)
  return text_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void Message::set_allocated_text(std::string* text) {
  if (text != nullptr) {
    
  } else {
    
  }
  text_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), text,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:simple.Message.text)
}

// uint64 size = 2;
inline void Message::clear_size() {
  size_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 Message::_internal_size() const {
  return size_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 Message::size() const {
  // @@protoc_insertion_point(field_get:simple.Message.size)
  return _internal_size();
}
inline void Message::_internal_set_size(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  
  size_ = value;
}
inline void Message::set_size(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_size(value);
  // @@protoc_insertion_point(field_set:simple.Message.size)
}

// -------------------------------------------------------------------

// EchoMessage

// string echo = 1;
inline void EchoMessage::clear_echo() {
  echo_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& EchoMessage::echo() const {
  // @@protoc_insertion_point(field_get:simple.EchoMessage.echo)
  return _internal_echo();
}
inline void EchoMessage::set_echo(const std::string& value) {
  _internal_set_echo(value);
  // @@protoc_insertion_point(field_set:simple.EchoMessage.echo)
}
inline std::string* EchoMessage::mutable_echo() {
  // @@protoc_insertion_point(field_mutable:simple.EchoMessage.echo)
  return _internal_mutable_echo();
}
inline const std::string& EchoMessage::_internal_echo() const {
  return echo_.Get();
}
inline void EchoMessage::_internal_set_echo(const std::string& value) {
  
  echo_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void EchoMessage::set_echo(std::string&& value) {
  
  echo_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:simple.EchoMessage.echo)
}
inline void EchoMessage::set_echo(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  echo_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:simple.EchoMessage.echo)
}
inline void EchoMessage::set_echo(const char* value,
    size_t size) {
  
  echo_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:simple.EchoMessage.echo)
}
inline std::string* EchoMessage::_internal_mutable_echo() {
  
  return echo_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* EchoMessage::release_echo() {
  // @@protoc_insertion_point(field_release:simple.EchoMessage.echo)
  return echo_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void EchoMessage::set_allocated_echo(std::string* echo) {
  if (echo != nullptr) {
    
  } else {
    
  }
  echo_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), echo,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:simple.EchoMessage.echo)
}

// uint64 size = 2;
inline void EchoMessage::clear_size() {
  size_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 EchoMessage::_internal_size() const {
  return size_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 EchoMessage::size() const {
  // @@protoc_insertion_point(field_get:simple.EchoMessage.size)
  return _internal_size();
}
inline void EchoMessage::_internal_set_size(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  
  size_ = value;
}
inline void EchoMessage::set_size(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_size(value);
  // @@protoc_insertion_point(field_set:simple.EchoMessage.size)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace simple

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_echo_2eproto
