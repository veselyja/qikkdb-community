// <auto-generated>
//     Generated by the protocol buffer compiler.  DO NOT EDIT!
//     source: Message/CSVImportMessage.proto
// </auto-generated>
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
namespace ColmnarDB.NetworkClient.Message {

  /// <summary>Holder for reflection information generated from Message/CSVImportMessage.proto</summary>
  public static partial class CSVImportMessageReflection {

    #region Descriptor
    /// <summary>File descriptor for Message/CSVImportMessage.proto</summary>
    public static pbr::FileDescriptor Descriptor {
      get { return descriptor; }
    }
    private static pbr::FileDescriptor descriptor;

    static CSVImportMessageReflection() {
      byte[] descriptorData = global::System.Convert.FromBase64String(
          string.Concat(
            "Ch5NZXNzYWdlL0NTVkltcG9ydE1lc3NhZ2UucHJvdG8SH0NvbG1uYXJEQi5O",
            "ZXR3b3JrQ2xpZW50Lk1lc3NhZ2UiigEKEENTVkltcG9ydE1lc3NhZ2USFAoM",
            "RGF0YWJhc2VOYW1lGAEgASgJEg8KB0NTVk5hbWUYAiABKAkSDwoHUGF5bG9h",
            "ZBgDIAEoCRI+CgtDb2x1bW5UeXBlcxgEIAMoDjIpLkNvbG1uYXJEQi5OZXR3",
            "b3JrQ2xpZW50Lk1lc3NhZ2UuRGF0YVR5cGUq1AIKCERhdGFUeXBlEg0KCUNP",
            "TlNUX0lOVBAAEhgKC0NPTlNUX0VSUk9SEP///////////wESDgoKQ09OU1Rf",
            "TE9ORxABEg8KC0NPTlNUX0ZMT0FUEAISEAoMQ09OU1RfRE9VQkxFEAMSDwoL",
            "Q09OU1RfUE9JTlQQBBIRCg1DT05TVF9QT0xZR09OEAUSEAoMQ09OU1RfU1RS",
            "SU5HEAYSEAoMQ09OU1RfSU5UOF9UEAcSDgoKQ09MVU1OX0lOVBAIEg8KC0NP",
            "TFVNTl9MT05HEAkSEAoMQ09MVU1OX0ZMT0FUEAoSEQoNQ09MVU1OX0RPVUJM",
            "RRALEhAKDENPTFVNTl9QT0lOVBAMEhIKDkNPTFVNTl9QT0xZR09OEA0SEQoN",
            "Q09MVU1OX1NUUklORxAOEhEKDUNPTFVNTl9JTlQ4X1QQDxISCg5EQVRBX1RZ",
            "UEVfU0laRRAQYgZwcm90bzM="));
      descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
          new pbr::FileDescriptor[] { },
          new pbr::GeneratedClrTypeInfo(new[] {typeof(global::ColmnarDB.NetworkClient.Message.DataType), }, new pbr::GeneratedClrTypeInfo[] {
            new pbr::GeneratedClrTypeInfo(typeof(global::ColmnarDB.NetworkClient.Message.CSVImportMessage), global::ColmnarDB.NetworkClient.Message.CSVImportMessage.Parser, new[]{ "DatabaseName", "CSVName", "Payload", "ColumnTypes" }, null, null, null)
          }));
    }
    #endregion

  }
  #region Enums
  public enum DataType {
    [pbr::OriginalName("CONST_INT")] ConstInt = 0,
    [pbr::OriginalName("CONST_ERROR")] ConstError = -1,
    [pbr::OriginalName("CONST_LONG")] ConstLong = 1,
    [pbr::OriginalName("CONST_FLOAT")] ConstFloat = 2,
    [pbr::OriginalName("CONST_DOUBLE")] ConstDouble = 3,
    [pbr::OriginalName("CONST_POINT")] ConstPoint = 4,
    [pbr::OriginalName("CONST_POLYGON")] ConstPolygon = 5,
    [pbr::OriginalName("CONST_STRING")] ConstString = 6,
    [pbr::OriginalName("CONST_INT8_T")] ConstInt8T = 7,
    [pbr::OriginalName("COLUMN_INT")] ColumnInt = 8,
    [pbr::OriginalName("COLUMN_LONG")] ColumnLong = 9,
    [pbr::OriginalName("COLUMN_FLOAT")] ColumnFloat = 10,
    [pbr::OriginalName("COLUMN_DOUBLE")] ColumnDouble = 11,
    [pbr::OriginalName("COLUMN_POINT")] ColumnPoint = 12,
    [pbr::OriginalName("COLUMN_POLYGON")] ColumnPolygon = 13,
    [pbr::OriginalName("COLUMN_STRING")] ColumnString = 14,
    [pbr::OriginalName("COLUMN_INT8_T")] ColumnInt8T = 15,
    [pbr::OriginalName("DATA_TYPE_SIZE")] Size = 16,
  }

  #endregion

  #region Messages
  public sealed partial class CSVImportMessage : pb::IMessage<CSVImportMessage> {
    private static readonly pb::MessageParser<CSVImportMessage> _parser = new pb::MessageParser<CSVImportMessage>(() => new CSVImportMessage());
    private pb::UnknownFieldSet _unknownFields;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pb::MessageParser<CSVImportMessage> Parser { get { return _parser; } }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pbr::MessageDescriptor Descriptor {
      get { return global::ColmnarDB.NetworkClient.Message.CSVImportMessageReflection.Descriptor.MessageTypes[0]; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    pbr::MessageDescriptor pb::IMessage.Descriptor {
      get { return Descriptor; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CSVImportMessage() {
      OnConstruction();
    }

    partial void OnConstruction();

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CSVImportMessage(CSVImportMessage other) : this() {
      databaseName_ = other.databaseName_;
      cSVName_ = other.cSVName_;
      payload_ = other.payload_;
      columnTypes_ = other.columnTypes_.Clone();
      _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CSVImportMessage Clone() {
      return new CSVImportMessage(this);
    }

    /// <summary>Field number for the "DatabaseName" field.</summary>
    public const int DatabaseNameFieldNumber = 1;
    private string databaseName_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string DatabaseName {
      get { return databaseName_; }
      set {
        databaseName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "CSVName" field.</summary>
    public const int CSVNameFieldNumber = 2;
    private string cSVName_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string CSVName {
      get { return cSVName_; }
      set {
        cSVName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "Payload" field.</summary>
    public const int PayloadFieldNumber = 3;
    private string payload_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string Payload {
      get { return payload_; }
      set {
        payload_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "ColumnTypes" field.</summary>
    public const int ColumnTypesFieldNumber = 4;
    private static readonly pb::FieldCodec<global::ColmnarDB.NetworkClient.Message.DataType> _repeated_columnTypes_codec
        = pb::FieldCodec.ForEnum(34, x => (int) x, x => (global::ColmnarDB.NetworkClient.Message.DataType) x);
    private readonly pbc::RepeatedField<global::ColmnarDB.NetworkClient.Message.DataType> columnTypes_ = new pbc::RepeatedField<global::ColmnarDB.NetworkClient.Message.DataType>();
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public pbc::RepeatedField<global::ColmnarDB.NetworkClient.Message.DataType> ColumnTypes {
      get { return columnTypes_; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override bool Equals(object other) {
      return Equals(other as CSVImportMessage);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public bool Equals(CSVImportMessage other) {
      if (ReferenceEquals(other, null)) {
        return false;
      }
      if (ReferenceEquals(other, this)) {
        return true;
      }
      if (DatabaseName != other.DatabaseName) return false;
      if (CSVName != other.CSVName) return false;
      if (Payload != other.Payload) return false;
      if(!columnTypes_.Equals(other.columnTypes_)) return false;
      return Equals(_unknownFields, other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override int GetHashCode() {
      int hash = 1;
      if (DatabaseName.Length != 0) hash ^= DatabaseName.GetHashCode();
      if (CSVName.Length != 0) hash ^= CSVName.GetHashCode();
      if (Payload.Length != 0) hash ^= Payload.GetHashCode();
      hash ^= columnTypes_.GetHashCode();
      if (_unknownFields != null) {
        hash ^= _unknownFields.GetHashCode();
      }
      return hash;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override string ToString() {
      return pb::JsonFormatter.ToDiagnosticString(this);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void WriteTo(pb::CodedOutputStream output) {
      if (DatabaseName.Length != 0) {
        output.WriteRawTag(10);
        output.WriteString(DatabaseName);
      }
      if (CSVName.Length != 0) {
        output.WriteRawTag(18);
        output.WriteString(CSVName);
      }
      if (Payload.Length != 0) {
        output.WriteRawTag(26);
        output.WriteString(Payload);
      }
      columnTypes_.WriteTo(output, _repeated_columnTypes_codec);
      if (_unknownFields != null) {
        _unknownFields.WriteTo(output);
      }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int CalculateSize() {
      int size = 0;
      if (DatabaseName.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(DatabaseName);
      }
      if (CSVName.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(CSVName);
      }
      if (Payload.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(Payload);
      }
      size += columnTypes_.CalculateSize(_repeated_columnTypes_codec);
      if (_unknownFields != null) {
        size += _unknownFields.CalculateSize();
      }
      return size;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(CSVImportMessage other) {
      if (other == null) {
        return;
      }
      if (other.DatabaseName.Length != 0) {
        DatabaseName = other.DatabaseName;
      }
      if (other.CSVName.Length != 0) {
        CSVName = other.CSVName;
      }
      if (other.Payload.Length != 0) {
        Payload = other.Payload;
      }
      columnTypes_.Add(other.columnTypes_);
      _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(pb::CodedInputStream input) {
      uint tag;
      while ((tag = input.ReadTag()) != 0) {
        switch(tag) {
          default:
            _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
            break;
          case 10: {
            DatabaseName = input.ReadString();
            break;
          }
          case 18: {
            CSVName = input.ReadString();
            break;
          }
          case 26: {
            Payload = input.ReadString();
            break;
          }
          case 34:
          case 32: {
            columnTypes_.AddEntriesFrom(input, _repeated_columnTypes_codec);
            break;
          }
        }
      }
    }

  }

  #endregion

}

#endregion Designer generated code
