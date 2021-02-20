#pragma once
template<typename T>
class Singleton
{
	static T* _instance;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() : _instance(nullptr) {};
	~Singleton() {};

public:
	static T& GetInstance() {
		if (_instance == nullptr) {
			_instance = new T;
		}
		return _instance;
	}

	static void Release() {
		if (_instance == nullptr) return;
		delete _instance;
		_instance = nullptr;
	}
};