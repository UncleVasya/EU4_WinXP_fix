#include "stdafx.h"

extern "C" PCSTR WSAAPI inet_ntop(INT Family, PVOID pAddr, PSTR pStringBuf, size_t StringBufSize) {
	boost::system::error_code ec;

	return boost::asio::detail::socket_ops::inet_ntop(Family, pAddr, pStringBuf, StringBufSize, 0, ec);
}

extern "C" INT WSAAPI inet_pton(INT Family, PCTSTR pszAddrString, PVOID  pAddrBuf) {
	boost::system::error_code ec;

	return boost::asio::detail::socket_ops::inet_pton(Family, pszAddrString, pAddrBuf, 0, ec);
}
