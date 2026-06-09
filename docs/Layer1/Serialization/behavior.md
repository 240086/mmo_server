Serialization Behavior Contract

serialize_is_deterministic

同样输入

必须产生完全一致字节流

------------------------------------------------

deserialize_restores_original_value

反序列化后

值必须一致

------------------------------------------------

serialization_never_allocates_implicitly

序列化过程

禁止隐藏堆分配

------------------------------------------------

serialization_is_append_only

Writer

仅允许尾部写入

------------------------------------------------

reader_never_reads_out_of_bounds

读取越界

返回失败

------------------------------------------------

string_length_is_encoded

字符串

长度前缀 + 数据

------------------------------------------------

endianness_is_fixed

V1

Little Endian

------------------------------------------------

serialization_is_binary_stable

协议升级前

二进制布局保持稳定

------------------------------------------------

primitive_serialization_is_O1

基础类型

O(1)

deserialize_is_rollback_safe

serialized_size_is_predictable
Packet预分配
RPC Buffer预分配
