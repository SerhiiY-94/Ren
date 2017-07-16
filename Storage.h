#pragma once

#include "SparseArray.h"

namespace ren {
    template<typename T>
    class StorageRef;

    template<typename T>
    class Storage : public SparseArray<T> {
    public:
        template<class... Args>
        StorageRef<T> Add(Args &&... args) {
            size_t index = SparseArray<T>::Add(args...);
            return { this, index };
        }
    };

    class RefCounter {
    protected:
        template<class T> friend class StorageRef;

        void add_ref() { ++counter_; }
        bool release() { return --counter_ == 0; }

        RefCounter() : counter_(0) {}
        RefCounter(const RefCounter&) : counter_(0) {}
        RefCounter &operator=(const RefCounter&) { return *this; }
        //~RefCounter() {}

        //RefCounter(const RefCounter &) = delete;
        RefCounter(RefCounter &&rhs) : counter_(rhs.counter_) { rhs.counter_ = 0; }
        //RefCounter &operator=(const RefCounter &) = delete;
        RefCounter &operator=(RefCounter &&rhs) { counter_ = rhs.counter_; rhs.counter_ = 0; return *this; }
    private:
        mutable unsigned counter_;
    };

    template <class T>
    class StorageRef {
        Storage<T> *storage_;
        size_t index_;
    public:
        StorageRef() : storage_(nullptr), index_(0) {}
        StorageRef(Storage<T> *storage, size_t index) : storage_(storage), index_(index) {
            if (storage_) {
                T *p = storage_->Get(index_);
                p->add_ref();
            }
        }
        ~StorageRef() {
            Release();
        }

        StorageRef(const StorageRef &rhs) {
            storage_ = rhs.storage_;
            index_ = rhs.index_;

            if (storage_) {
                T *p = storage_->Get(index_);
                p->add_ref();
            }
        }

        StorageRef(StorageRef &&rhs) {
            storage_ = rhs.storage_;
            rhs.storage_ = nullptr;
            index_ = rhs.index_;
            rhs.index_ = 0;
        }

        StorageRef &operator=(const StorageRef &rhs) {
            Release();

            storage_ = rhs.storage_;
            index_ = rhs.index_;

            if (storage_) {
                T *p = storage_->Get(index_);
                p->add_ref();
            }

            return *this;
        }

        StorageRef &operator=(StorageRef &&rhs) {
            Release();

            storage_ = rhs.storage_;
            rhs.storage_ = nullptr;
            index_ = rhs.index_;
            rhs.index_ = 0;

            return *this;
        }

        T *operator->() {
            assert(storage_);
            return storage_->Get(index_);
        }

        const T *operator->() const {
            assert(storage_);
            return storage_->Get(index_);
        }

        T &operator*() {
            assert(storage_);
            return *storage_->Get(index_);
        }

        const T &operator*() const {
            assert(storage_);
            return *storage_->Get(index_);
        }

        T *get() {
            assert(storage_);
            return storage_->Get(index_);
        }

        const T *get() const {
            assert(storage_);
            return storage_->Get(index_);
        }

        operator bool() const {
            return storage_ != nullptr;
        }

        void Release() {
            if (storage_) {
                T *p = storage_->Get(index_);
                if (p->release()) {
                    storage_->Remove(index_);
                }
                storage_ = nullptr;
                index_ = 0;
            }
        }
    };
}
