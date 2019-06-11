// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: messages/SetDatabaseMessage.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_messages_2fSetDatabaseMessage_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_messages_2fSetDatabaseMessage_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3008000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3008000 < PROTOBUF_MIN_PROTOC_VERSION
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
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_messages_2fSetDatabaseMessage_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_messages_2fSetDatabaseMessage_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_messages_2fSetDatabaseMessage_2eproto;
namespace ColmnarDB {
namespace NetworkClient {
namespace Message {
class SetDatabaseMessage;
class SetDatabaseMessageDefaultTypeInternal;
extern SetDatabaseMessageDefaultTypeInternal _SetDatabaseMessage_default_instance_;
}  // namespace Message
}  // namespace NetworkClient
}  // namespace ColmnarDB
PROTOBUF_NAMESPACE_OPEN
template<> ::ColmnarDB::NetworkClient::Message::SetDatabaseMessage* Arena::CreateMaybeMessage<::ColmnarDB::NetworkClient::Message::SetDatabaseMessage>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace ColmnarDB {
namespace NetworkClient {
namespace Message {

// ===================================================================

class SetDatabaseMessage :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ColmnarDB.NetworkClient.Message.SetDatabaseMessage) */ {
 public:
  SetDatabaseMessage();
  virtual ~SetDatabaseMessage();

  SetDatabaseMessage(const SetDatabaseMessage& from);
  SetDatabaseMessage(SetDatabaseMessage&& from) noexcept
    : SetDatabaseMessage() {
    *this = ::std::move(from);
  }

  inline SetDatabaseMessage& operator=(const SetDatabaseMessage& from) {
    CopyFrom(from);
    return *this;
  }
  inline SetDatabaseMessage& operator=(SetDatabaseMessage&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
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
  static const SetDatabaseMessage& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SetDatabaseMessage* internal_default_instance() {
    return reinterpret_cast<const SetDatabaseMessage*>(
               &_SetDatabaseMessage_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(SetDatabaseMessage* other);
  friend void swap(SetDatabaseMessage& a, SetDatabaseMessage& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SetDatabaseMessage* New() const final {
    return CreateMaybeMessage<SetDatabaseMessage>(nullptr);
  }

  SetDatabaseMessage* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<SetDatabaseMessage>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const SetDatabaseMessage& from);
  void MergeFrom(const SetDatabaseMessage& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(SetDatabaseMessage* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ColmnarDB.NetworkClient.Message.SetDatabaseMessage";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_messages_2fSetDatabaseMessage_2eproto);
    return ::descriptor_table_messages_2fSetDatabaseMessage_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string DatabaseName = 1;
  void clear_databasename();
  static const int kDatabaseNameFieldNumber = 1;
  const std::string& databasename() const;
  void set_databasename(const std::string& value);
  void set_databasename(std::string&& value);
  void set_databasename(const char* value);
  void set_databasename(const char* value, size_t size);
  std::string* mutable_databasename();
  std::string* release_databasename();
  void set_allocated_databasename(std::string* databasename);

  // @@protoc_insertion_point(class_scope:ColmnarDB.NetworkClient.Message.SetDatabaseMessage)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr databasename_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_messages_2fSetDatabaseMessage_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SetDatabaseMessage

// string DatabaseName = 1;
inline void SetDatabaseMessage::clear_databasename() {
  databasename_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& SetDatabaseMessage::databasename() const {
  // @@protoc_insertion_point(field_get:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
  return databasename_.GetNoArena();
}
inline void SetDatabaseMessage::set_databasename(const std::string& value) {
  
  databasename_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
}
inline void SetDatabaseMessage::set_databasename(std::string&& value) {
  
  databasename_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
}
inline void SetDatabaseMessage::set_databasename(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  databasename_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
}
inline void SetDatabaseMessage::set_databasename(const char* value, size_t size) {
  
  databasename_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
}
inline std::string* SetDatabaseMessage::mutable_databasename() {
  
  // @@protoc_insertion_point(field_mutable:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
  return databasename_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* SetDatabaseMessage::release_databasename() {
  // @@protoc_insertion_point(field_release:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
  
  return databasename_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void SetDatabaseMessage::set_allocated_databasename(std::string* databasename) {
  if (databasename != nullptr) {
    
  } else {
    
  }
  databasename_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), databasename);
  // @@protoc_insertion_point(field_set_allocated:ColmnarDB.NetworkClient.Message.SetDatabaseMessage.DatabaseName)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Message
}  // namespace NetworkClient
}  // namespace ColmnarDB

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_messages_2fSetDatabaseMessage_2eproto
