# **gRPC logger**
gRPC logger for high loaded services


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
- Поддерживает модель multiple-producer/single-consumer

### 3. Шаблонная реализация
```cpp
template <typename PackedObject, size_t N>
class RingBuffer
```
### 4. Гарантия плотной упаковки данных
- `PackedObject` должен быть **POD-типом** (Plain Old Data)
- Требуется для корректной работы с `memcpy` при чтении/записи
- Исключает выравнивание и виртуальные методы

