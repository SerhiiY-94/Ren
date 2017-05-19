#include "test_common.h"

#include "../Storage.h"

void test_storage() {
	struct MyObj : public ren::RefCounter { 
		int *ref;

		MyObj() : ref(nullptr) {}
		MyObj(int *r) : ref(r) {
			(*ref)++;
		}
		MyObj(const MyObj &rhs) = delete;
		MyObj(MyObj &&rhs) : ref(rhs.ref) {
			rhs.ref = nullptr;
		}
		MyObj &operator=(const MyObj &rhs) = delete;
		MyObj &operator=(MyObj &&rhs) {
			if (ref) {
				(*ref)--;
			}
			ref = rhs.ref;
			rhs.ref = nullptr;
			return (*this);
		}

		~MyObj() {
			if (ref) {
				(*ref)--;
			}
		}
	};

	{	// test create/delete
		ren::Storage<MyObj> my_obj_storage;
		int counter = 0;

		auto ref1 = my_obj_storage.Add(&counter);
		assert(counter == 1);
		ref1 = {};
		assert(counter == 0);
	}

	{	// test copy/move reference
		ren::Storage<MyObj> my_obj_storage;
		int counter = 0;

		auto ref1 = my_obj_storage.Add(&counter);
		assert(counter == 1);
		auto ref2 = ref1;
		assert(counter == 1);
		ref1 = {};
		assert(counter == 1);
		ref2 = {};
		assert(counter == 0);

        ref1 = my_obj_storage.Add(&counter);
        assert(counter == 1);
        ref2 = std::move(ref1);
        assert(counter == 1);
        ref2 = {};
        assert(counter == 0);
	}
}