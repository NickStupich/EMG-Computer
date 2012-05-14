#ifdef WIN32

private:
	wchar_t* _port;
	DWORD _baud;
	DataProtocol* _protocol;
	HANDLE _handle;
	bool _isOpen;

#endif