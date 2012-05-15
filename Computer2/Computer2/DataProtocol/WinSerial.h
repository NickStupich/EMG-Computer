private:
	wchar_t* _port;
	DWORD _baud;
	HANDLE _handle;
	bool _isOpen;
	HANDLE _readThreadHandle;