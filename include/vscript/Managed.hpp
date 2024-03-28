#pragma once
#include <functional>
#include <mutex>
#include <set>


namespace vs
{
 template <typename T, typename E, typename = std::enable_if_t<!std::is_base_of_v
            <T, E>>>
std::function<void(E *)> makeNewDeleter(std::function<void(T *)> oldDeleter,
                                        float n = 0) {
  return [oldDeleter](E *ptr) {
    oldDeleter(static_cast<T *>(ptr));
  };
}

template <typename T, typename E, typename = std::enable_if_t<std::is_base_of_v<
            T, E>>>
std::function<void(E *)> makeNewDeleter(std::function<void(T *)> oldDeleter) {
  return [oldDeleter](E *ptr) {
    oldDeleter(dynamic_cast<T *>(ptr));
  };
}

  template<typename Base,typename T,typename = std::enable_if_t<std::is_base_of_v<Base,T>>>
  Base * castToBase(T * data,float _ = 0)
 {
   return static_cast<Base*>(data);
 }

  template<typename Base,typename T,typename = std::enable_if_t<!std::is_base_of_v<Base,T>>>
  Base * castToBase(T * data)
 {
   return nullptr;
 }

template <class T>
class Ref;

template <class T>
class Managed;


struct RefBase {
  virtual void Clear(bool bCheckRef = true) {

  }

  virtual ~RefBase() = default;
};

struct ManagedBase {
  virtual void Clear() = 0;
  virtual ~ManagedBase() = default;
};

struct SharedInfo {
  uint64_t locks = 0;
  std::mutex mutex{};
  std::set<RefBase *> weak{};
  std::set<ManagedBase *> strong{};
  bool bIsPendingDelete = false;
};

template <class T>
struct ManagedBlock {
  friend class Managed<T>;
  friend class Ref<T>;
  T *data = nullptr;

  std::function<void(T *)> deleter = [](T *p) { delete p; };

  SharedInfo *shared = nullptr;

  void AddStrong(ManagedBase *ptr) const;
  void AddWeak(RefBase *ptr) const;
  void RemoveStrong(ManagedBase *ptr) const;
  void RemoveWeak(RefBase *ptr) const;
  void SetData(T *newData);

  bool IsUsable() const;
  void Lock() const;
  void Unlock();

  ManagedBlock();

  ManagedBlock(SharedInfo *inShared);

  ~ManagedBlock();
};

// Stores a reference to T, will deallocate T once T has no more Managed references
template <class T>
class Managed : public ManagedBase {
  ManagedBlock<T> *_block = nullptr;

protected:
  void UseBlock(ManagedBlock<T> *block);

public:
  Managed();
  Managed(std::nullptr_t);
  explicit Managed(ManagedBlock<T> *block);
  explicit Managed(SharedInfo *shared, T *data,
                   const std::function<void(T *)> &deleter);
  explicit Managed(T *data);
  explicit Managed(T *data, const std::function<void(T *)> &deleter);
  Managed(Managed &other);
  Managed(const Managed &other);
  Managed(Managed &&other) noexcept;
  // Initialize from a subclass of T
  template <typename E, typename = std::enable_if_t<std::is_base_of_v<T, E>>>
  Managed(const Managed<E> &other);
  Managed &operator=(const Managed &other);
  Managed &operator=(Managed &&other) noexcept;

  // Initialize from a subclass of T
  template <typename E>
  Managed &operator=(const Managed<E> &other);

  // Initialize from a subclass of T
  template <typename E>
  Managed &operator=(Managed<E> &&other) noexcept;

  T *Get();
  T *Get() const;

  // Convert T to a subclass E
  template <typename E, typename = std::enable_if_t<std::is_base_of_v<T, E>>>
  Managed<E> Cast() const;

  // Cast T to a valid type E
  template <typename E>
  Managed<E> CastStatic() const;

  bool operator==(const Managed &other) const;
  bool operator==(const Ref<T> &other) const;

  Ref<T> ToRef();
  Ref<T> ToRef() const;
operator Ref<T> ();
operator Ref<T> () const;
  // operator T*();
  // operator T*() const;
  T *operator->();
  T *operator->() const;

  bool IsValid() const;

  void Clear() override;

  Managed<T> &Swap(T *data);
  Managed<T> &Swap(T *data, std::function<void(T *)> deleter);

  ~Managed() override;
  friend class Ref<T>;
};




// A weak version of Managed that will not be considered when deciding to delete the pointer
template <class T>
class Ref : public RefBase {
  ManagedBlock<T> *_block = nullptr;

protected:
  void UseBlock(ManagedBlock<T> *block);

public:
  Ref();
  Ref(std::nullptr_t);
  explicit Ref(ManagedBlock<T> *block);
  Ref(const Ref &other);
  Ref(Ref &&other) noexcept;
  template <typename E, typename = std::enable_if_t<std::is_base_of_v<T, E>>>
  explicit Ref(const Ref<E> &other);

  Ref &operator=(const Ref &other);
  Ref &operator=(Ref &&other) noexcept;

  template <typename E, typename = std::enable_if_t<std::is_base_of_v<T, E>>>
  Ref &operator=(const Ref<E> &other);
  template <typename E, typename = std::enable_if_t<std::is_base_of_v<T, E>>>
  Ref &operator=(Ref<E> &&other) noexcept;

  template <typename E, typename = std::enable_if_t<std::is_base_of_v<T, E>>>
  Ref<E> Cast() const;

  template <typename E>
  Ref<E> CastStatic() const;

  [[nodiscard]] bool IsValid() const;

  bool operator==(const Managed<T> &other) const;
  bool operator==(const Ref<T> &other) const;

  Managed<T> Reserve();
  Managed<T> Reserve() const;

  void Clear(bool bCheckRef = true) override;
  ~Ref() override;
  friend class Managed<T>;
};

struct RefContainer {
  RefBase *ref = nullptr;

  virtual ~RefContainer() {
    delete ref;
  }
};


struct RefThisBase {
  
protected:
  friend ManagedBlock;
  RefContainer _managedWeak;
  virtual void OnRefSet();
};

inline void RefThisBase::OnRefSet()
{
}


template <class T>
struct RefThis : RefThisBase {

public:
  Ref<T> ToRef() const;
};


template <class T> void ManagedBlock<T>::AddStrong(ManagedBase *ptr) const {
  shared->strong.emplace(ptr);
}

template <class T> void ManagedBlock<T>::AddWeak(RefBase *ptr) const {
  shared->weak.emplace(ptr);
}

template <class T> void ManagedBlock<T>::RemoveStrong(ManagedBase *ptr) const {
  shared->strong.erase(ptr);
}

template <class T> void ManagedBlock<T>::RemoveWeak(RefBase *ptr) const {
  shared->weak.erase(ptr);
}

template <class T> void ManagedBlock<T>::SetData(T *newData) {
  data = newData;

  if(auto asBase = castToBase<RefThisBase>(data))
  {
    if(asBase->_managedWeak.ref == nullptr)
    {
      asBase->_managedWeak.ref = new Ref<T>(this);
      asBase->OnRefSet();
    }
  }
}

template <class T> bool ManagedBlock<T>::IsUsable() const {
  return shared && !shared->bIsPendingDelete;
}

template <class T> void ManagedBlock<T>::Lock() const {
  shared->mutex.lock();
}

template <class T> void ManagedBlock<T>::Unlock() {
  shared->mutex.unlock();

  if (shared->bIsPendingDelete && shared->mutex.try_lock()) {
    if (data != nullptr) {
      deleter(data);
      data = nullptr;
    }

    for (const auto &val : shared->strong) {
      val->Clear();
    }

    for (const auto &val : shared->weak) {
      val->Clear(false);
    }

    shared->strong.clear();
    shared->weak.clear();
    shared->mutex.unlock();

    delete this;
  }
}

template <class T> ManagedBlock<T>::ManagedBlock() {
  shared = new SharedInfo;
}

template <class T> ManagedBlock<T>::ManagedBlock(SharedInfo *inShared) {
  shared = inShared;
}

template <class T> ManagedBlock<T>::~ManagedBlock() {
  delete shared;
  shared = nullptr;
}

template <class T> void Managed<T>::UseBlock(ManagedBlock<T> *block) {
  Clear();
  _block = block;
  if (_block != nullptr && _block->IsUsable()) {
    _block->Lock();
    _block->AddStrong(this);
    _block->Unlock();
  }
}

template <class T> Managed<T>::Managed() {
  _block = nullptr;
}

template <class T> Managed<T>::Managed(std::nullptr_t) {
  _block = nullptr;
}

template <class T> Managed<T>::Managed(ManagedBlock<T> *block) {
  UseBlock(block);
}

template <class T> Managed<T>::Managed(SharedInfo *shared, T *data,
                                       const std::function<void(
                                           T *)> &deleter) {
  shared->mutex.lock();
  shared->strong.emplace(this);
  shared->mutex.unlock();
  _block = new ManagedBlock<T>(shared);
  _block->SetData(data);
  _block->deleter = deleter;
}

template <class T> Managed<T>::Managed(T *data) {
  _block = new ManagedBlock<T>;
  _block->AddStrong(this);
  _block->SetData(data);
  
}

template <class T> Managed<T>::Managed(T *data,
                                       const std::function<void(
                                           T *)> &deleter) {
  _block = new ManagedBlock<T>;
  _block->AddStrong(this);
  _block->SetData(data);
  _block->deleter = deleter;
}

template <class T> Managed<T>::Managed(Managed &other) {
  UseBlock(other._block);
}

template <class T> Managed<T>::Managed(const Managed &other) {
  UseBlock(other._block);
}

template <class T> Managed<T>::Managed(Managed &&other) noexcept {
  UseBlock(other._block);
}

template <class T> template <typename E, typename> Managed<T>::Managed(
    const Managed<E> &other) {
  *this = other.template CastStatic<T>();
}

template <class T> Managed<T> &Managed<T>::operator=(const Managed &other) {
  if (*this != other) {
    UseBlock(other._block);
  }
  return *this;
}

template <class T> Managed<T> &Managed<T>::operator
=(Managed &&other) noexcept {
  if (*this != other) {
    UseBlock(other._block);
  }
  return *this;
}

template <class T> template <typename E> Managed<T> &Managed<T>::
operator=(const Managed<E> &other) {
  if (*this != other) {
    *this = other.template CastStatic<T>();
  }
  return *this;
}

template <class T> template <typename E> Managed<T> &Managed<T>::
operator=(Managed<E> &&other) noexcept {
  if (*this != other) {
    *this = other.template CastStatic<T>();
  }
  return *this;
}

template <class T> T *Managed<T>::Get() {
  if (_block == nullptr || !_block->IsUsable())
    return nullptr;
  return _block->data;
}

template <class T> T *Managed<T>::Get() const {
  if (_block == nullptr || !_block->IsUsable())
    return nullptr;
  return _block->data;
}

template <class T> template <typename E, typename> Managed<E> Managed<
  T>::Cast() const {
  if (_block == nullptr)
    return {};

  if (!_block->IsUsable())
    return {};

  if (auto dCasted = dynamic_cast<E *>(_block->data)) {
    auto d = _block->deleter;
    return Managed<E>(_block->shared, dCasted,
                      makeNewDeleter<T, E>(_block->deleter));
  }
  return {};
}

template <class T> template <typename E> Managed<E> Managed<
  T>::CastStatic() const {
  if (_block == nullptr)
    return {};

  if (!_block->IsUsable())
    return {};
  auto d = _block->deleter;
  return Managed<E>(_block->shared, static_cast<E *>(_block->data),
                    [d](E *ptr) {
                      d(static_cast<T *>(ptr));
                    });
}

template <class T> bool Managed<T>::operator==(const Managed &other) const {
  return _block == other._block;
}

template <class T> bool Managed<T>::operator==(const Ref<T> &other) const {
  return _block == other.Reserve()._block;
}

template <class T> Ref<T> Managed<T>::ToRef() {
  return Ref<T>(_block);
}

template <class T> Ref<T> Managed<T>::ToRef() const {
  return Ref<T>(_block);
}

    template<class T>
    Managed<T>::operator Ref<T>() {
        return ToRef();
    }

    template<class T>
    Managed<T>::operator Ref<T>() const {
        return ToRef();
    }

template <class T> T *Managed<T>::operator->() {
  if (_block == nullptr || !_block->IsUsable())
    return nullptr;
  return _block->data;
}

template <class T> T *Managed<T>::operator->() const {
  if (_block == nullptr || !_block->IsUsable())
    return nullptr;
  return _block->data;
}

template <class T> bool Managed<T>::IsValid() const {
  if (_block == nullptr || !_block->IsUsable())
    return false;
  return _block->data != nullptr;
}

template <class T> void Managed<T>::Clear() {
  if (_block != nullptr && _block->IsUsable()) {
    _block->Lock();
    _block->shared->strong.erase(this);
    if (_block->shared->strong.empty()) {
      _block->shared->bIsPendingDelete = true;
    }
    _block->Unlock();
  }

  _block = nullptr;
}

template <class T> Managed<T> &Managed<T>::Swap(T *data) {
  UseBlock(new ManagedBlock<T>);
  return *this;
}

template <class T> Managed<T>::~Managed() {
  Managed<T>::Clear();
}

template <class T> void Ref<T>::UseBlock(ManagedBlock<T> *block) {
  Clear();
  _block = block;
  if (_block != nullptr && _block->IsUsable()) {
    _block->Lock();
    _block->AddWeak(this);
    _block->Unlock();
  }
}

template <class T> Ref<T>::Ref() {
  _block = nullptr;
}

template <class T>
Ref<T>::Ref(std::nullptr_t) {
  _block = nullptr;
}

template <class T> Ref<T>::Ref(ManagedBlock<T> *block) {
  UseBlock(block);
}

template <class T> Ref<T>::Ref(const Ref &other) {
  UseBlock(other._block);
}

template <class T> Ref<T>::Ref(Ref &&other) noexcept {
  UseBlock(other._block);
}

template <class T> template <typename E, typename> Ref<T>::Ref(
    const Ref<E> &other) {
  *this = other.template Cast<T>();
}

template <class T> Ref<T> &Ref<T>::operator=(
    const Ref &other) {
  if (*this != other) {
    UseBlock(other._block);
  }
  return *this;
}

template <class T> Ref<T> &Ref<T>::operator=(
    Ref &&other) noexcept {
  if (*this != other) {
    UseBlock(other._block);
  }
  return *this;
}

template <class T> template <typename E, typename> Ref<T> &Ref<
  T>::operator=(const Ref<E> &other) {
  *this = other.template Cast<T>();
  return *this;
}

template <class T> template <typename E, typename> Ref<T> &Ref<
  T>::operator=(Ref<E> &&other) noexcept {
  *this = other.template Cast<T>();
  return *this;
}

template <class T> template <typename E, typename> [[nodiscard]] Ref<E> Ref<
  T>::Cast() const {
  return Reserve().template Cast<E>().ToRef();
}

template <class T> template <typename E> Ref<E> Ref<T>::CastStatic() const {
  return Reserve().template CastStatic<E>().ToRef();
}

template <class T> bool Ref<T>::IsValid() const {
  return _block != nullptr && _block->IsUsable() && _block->data != nullptr;
}

template <class T>
bool Ref<T>::operator==(const Ref<T> &other) const {
  return _block == other._block;
}

template <class T>
bool Ref<T>::operator==(const Managed<T> &other) const {
  return _block == other._block;
}

template <class T> Managed<T> Ref<T>::Reserve() {
  return Managed<T>(_block);
}

template <class T> Managed<T> Ref<T>::Reserve() const {
  return Managed<T>(_block);
}

template <class T> void Ref<T>::Clear(bool bCheckRef) {
  if (bCheckRef) {
    if (_block != nullptr) {
      if (_block->IsUsable()) {
        _block->Lock();
        _block->shared->weak.erase(this);
        _block->Unlock();
      } else {
        _block->shared->weak.erase(this);
      }
    }
  }
  _block = nullptr;
}

template <class T> Ref<T>::~Ref() {
  Ref<T>::Clear();
}

template <class T> Ref<T> RefThis<T>::ToRef() const {
  if (!_managedWeak.ref) {
    return {};
  }
  Ref<T> r = *static_cast<Ref<T> *>(_managedWeak.ref);
  return r;
}

template <typename T>
Managed<T> manage(T *data, std::function<void(T *)> deleter) {
  return Managed<T>(data, deleter);
}

template <typename T>
Managed<T> manage(T *data) {
  return Managed<T>(data);
}

template <typename T, typename... Args>
  std::enable_if_t<std::is_constructible_v<T, Args...>, Managed<T>> manage(Args... args) {
  return Managed<T>(new T(std::forward<Args>(args)...));
}
}
