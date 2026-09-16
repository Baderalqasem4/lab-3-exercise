#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER
#include <cassert>
#include <utility>

class ControlBlockBase {
public:
    ControlBlockBase() : mRefCount(1) {}; // TODO: implement the default constructor.

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() = default; // TODO: implement the destructor.

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // TODO: increment refcount by 1 and return result.
        mRefCount++;
        return mRefCount;
    }

    long decrement()
    {
        // TODO: decrement refcount by 1 and return result.
        assert(mRefCount > 0);
        mRefCount--;
        return mRefCount;
    }

    long refCount() const
    {
        // TODO: just return the refcount.
        return mRefCount;
    }

    private:
    // TODO: add field(s) which both control block types need to have
    long mRefCount;

};

template <typename T>
class ControlBlock : public ControlBlockBase {
    public: 
        ControlBlock(T* ptr) : mManaged(ptr) {}
        ~ControlBlock() override {
            delete mManaged;
        }

        void* managedAddress() override {
            return static_cast<void*>(mManaged);
        }

    private: T* mManaged;

};

template <typename T>
class SharedPtr {
    public:
        // empty state so a SharedPtr can exist before it manages anything
        SharedPtr() : mStoredPtr(nullptr), mControl(nullptr) {}

        // refcount starts at 1 because whoever calls it is the first owner
        explicit SharedPtr(T* ptr) : mStoredPtr(ptr), mControl(new ControlBlock<T>(ptr)) {}

        // destructor
        // only delete the control block if we were the last owner
        ~SharedPtr() {
            if (mControl) {
                if (mControl->decrement() == 0) {
                    delete mControl;
                }
            }
        } 

    // we are sharing the same resource as other now, so increment since there is one more owner
    SharedPtr(const SharedPtr& other) : mStoredPtr(other.mStoredPtr), mControl(other.mControl) {
        if (mControl) {
            mControl->increment();
        }
    }

    // relable owner so no need to increment
    SharedPtr(SharedPtr&& other) : mStoredPtr(other.mStoredPtr), mControl(other.mControl) {
        other.mStoredPtr = nullptr;
        other.mControl = nullptr;
    }

    // swap which object holds what
    void swap(SharedPtr& other) {
        std::swap(mStoredPtr, other.mStoredPtr);
        std::swap(mControl, other.mControl);
    }


    SharedPtr& operator=(const SharedPtr& other) {
        SharedPtr otherCopy(other); // make full copy of the other first
        swap(otherCopy);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        swap(other);
        other.reset(); // empty other out and release old resource right away
        return *this;
    }

    void reset() {
        SharedPtr().swap(*this); // swap with a new empty SharedPtr
    }

    void reset(T* other) {
        if (other == mStoredPtr) {
            return; // already managing this pointer
        }
        SharedPtr(other).swap(*this);
    }

    T& operator*() const {
        assert(mStoredPtr);
        return *mStoredPtr;
    }

    T* operator->() const {
        assert(mStoredPtr);
        return mStoredPtr;
    }

    T* get() const {
        return mStoredPtr;
    }

    bool operator==(const SharedPtr& other) const {
        return mStoredPtr == other.mStoredPtr;
    }

    explicit operator bool() const {
        return mStoredPtr != nullptr;
    }

    long useCount() const {
        return mControl ? mControl->refCount() : 0;
    }

    private: T* mStoredPtr;
    ControlBlockBase* mControl;

    template <typename U>
    friend class SharedPtr;
};

template <typename T, typename... Args>

SharedPtr<T> makeSharedBasic(Args&&... args) {
    T* raw = new T(std::forward<Args>(args)...);
    return SharedPtr<T>(raw);
};

#endif