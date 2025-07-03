# **gRPC logger**
gRPC logger for high loaded services


---

### **ToDo:**
 - batch write to ring buffer
 - multiple consumer/producer in ring buffer
### **ToThink about:**
 - do i need ring buffer?
 - creat PackedEntry right in ring buffer
 - paged RingBuffer, like memory pages (with sizeof PackedEntry)

---
## **RingBuffer.** Ключевые особенности

### 1. Реализация на разделяемой памяти
- Использует `mmap` для создания виртуального адресного пространства
- Применяет технику двойного отображения (double-mapping) для бесшовного перехода
- Поддерживает две платформы:
  - Linux: через `memfd_create`
  - macOS: через `shm_open`

### 2. Потокобезопасность
- Атомарные операции через `std::atomic`
- Lock-free реализация для операций чтения/записи
- Поддерживает модель single-producer/single-consumer

### 3. Шаблонная реализация
```cpp
template <typename PackedObject, size_t N>
class RingBuffer
```
### 4. Гарантия плотной упаковки данных
- `PackedObject` должен быть **POD-типом** (Plain Old Data)
- Требуется для корректной работы с `memcpy` при чтении/записи
- Исключает выравнивание и виртуальные методы

