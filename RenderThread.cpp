#include "RenderThread.h"

#include <thread>

ren::TaskList::TaskList() {
	done_event = std::make_shared<std::atomic_bool>();
	*(std::atomic_bool*)done_event.get() = false;
}

void ren::TaskList::Submit(RenderThread *r) {
	r->AddTaskList(std::move(*this));
}

void ren::TaskList::Wait() {
#ifndef __EMSCRIPTEN__
	while (!*(std::atomic_bool*)done_event.get()) {
		std::this_thread::yield();
	}
#endif
}

void ren::RenderThread::AddTaskList(TaskList &&list) {
#ifndef __EMSCRIPTEN__
	std::lock_guard<std::mutex> lck(add_list_mtx_);
	task_lists_.Push(std::move(list));
#else
	for (auto &t : list) {
		t.func(t.arg);
	}
	*(std::atomic_bool*)list.done_event.get() = true;
#endif
}

void ren::RenderThread::AddSingleTask(TaskFunc func, void *arg) {
#ifndef __EMSCRIPTEN__
	TaskList list;
	list.push_back({ func, arg });
	list.Submit(this);
#else
	func(arg);
#endif
}

void ren::RenderThread::ProcessSingleTask(TaskFunc func, void *arg) {
#ifndef __EMSCRIPTEN__
	TaskList list;
	list.push_back({ func, arg });
	list.Submit(this);
	list.Wait();
#else
	func(arg);
#endif
}

bool ren::RenderThread::ProcessTasks() {
#ifndef __EMSCRIPTEN__
	TaskList list;
	while (task_lists_.Pop(list)) {
		for (auto &t : list) {
			t.func(t.arg);
		}
		*(std::atomic_bool*)list.done_event.get() = true;
		return true;
	}
	return false;
#else
    return false;
#endif
}