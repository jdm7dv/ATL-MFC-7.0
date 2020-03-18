// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#pragma once
#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "ws2_32.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlfile.h>
#include <atlmime.h>
#include <atlsmtputil.h>



// SMTP Return Codes
#define ATLSMTP_MAIL_SUCCESS      250
#define ATLSMTP_RCPT_SUCCESS      250
#define ATLSMTP_RCPT_NOT_LOCAL    251
#define ATLSMTP_DATA_INTERMEDIATE 354

#define ATLSMTP_CONN_SUCC "220"
#define ATLSMTP_HELO_SUCC "250"
#define ATLSMTP_MAIL_SUCC "250"
#define ATLSMTP_RCPT_SUCC "250"
#define ATLSMTP_RCPT_NLOC "251"
#define ATLSMTP_DATA_INTM "354"
#define ATLSMTP_DATA_SUCC "250"
#define ATLSMTP_RSET_SUCC "250"

// SMTP flags
#define ATLSMTP_DUMP_SENDER 1
#define ATLSMTP_DUMP_RECIPS 2
#define ATLSMTP_FOR_SEND    4


struct CSMTPWSAStartup
{
	bool m_bInit;

	CSMTPWSAStartup() throw()
		:m_bInit(false)
	{
	}

	~CSMTPWSAStartup() throw()
	{
		Uninit();
	}

	bool Init() throw()
	{
		if (m_bInit)
			return true;

		WSADATA wsadata;
		if (WSAStartup(ATLSMTP_WSA_VERSION, &wsadata))
			return false;
		m_bInit = true;
		ATLASSERT(wsadata.wHighVersion >= 2);
		return true;
	}

	bool Uninit() throw()
	{
		if (m_bInit)
			if (WSACleanup())
				return false;
		m_bInit = false;
		return true;
	}
};

__declspec(selectany) CSMTPWSAStartup _g_smtp_init;

namespace ATL {

class CSMTPConnection
{
protected:

	// the socket
	SOCKET m_hSocket;

	// the OVERLAPPED struct
	OVERLAPPED m_Overlapped;

public:

	CSMTPConnection() throw()
		:m_hSocket(INVALID_SOCKET)
	{
		// initialize the OVERLAPPED struct
		memset(&m_Overlapped, 0, sizeof(OVERLAPPED));
	}

	~CSMTPConnection() throw()
	{
		Disconnect();
	}

	// Attempt to connect to the socket
	// lpszHostName - the host name to connect to
	BOOL Connect(LPCTSTR lpszHostName) throw()
	{
		ATLASSERT(lpszHostName != NULL);

		// If we're already connected
		if (Connected())
			return FALSE;

		sockaddr_in sockAddr;
		// initialize the sockaddr_in structure
		sockAddr.sin_family = PF_INET;
		sockAddr.sin_port = htons(IPPORT_SMTP);

		// default
		sockAddr.sin_addr.s_addr = INADDR_ANY;

		// WSAStartup should return 0 on success
//		WSADATA wsaData;
//		if (WSAStartup(ATLSMTP_WSA_VERSION, &wsaData))
//			return FALSE;

		if (!_g_smtp_init.Init())
			return FALSE;

		CT2CA lpszHostNameA(lpszHostName);
		// see if it's a dotted IP address string
		ULONG ulAddr = inet_addr (lpszHostNameA);
		HOSTENT* pHost;
		if (ulAddr == INADDR_NONE)
		{
			pHost = gethostbyname(lpszHostNameA);

			if (!pHost)
			{
				//gethostbyname failed
				return FALSE;
			}
			ulAddr = *((ULONG FAR*)pHost->h_addr);
		}

		sockAddr.sin_addr.s_addr = ulAddr;

		// create the socket
		m_hSocket = socket(PF_INET, SOCK_STREAM, 0);
		
		if (m_hSocket == INVALID_SOCKET)
		{
			return FALSE;
		}

		BOOL bRet = TRUE;
		// connect should return 0 on success
		if (connect(m_hSocket, (LPSOCKADDR)&sockAddr, sizeof(sockaddr)))
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				bRet = FALSE;
			}
		}

		// Create an event for asynchronous I/O
		ATLASSERT(m_Overlapped.hEvent == NULL);
		m_Overlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		if (m_Overlapped.hEvent == NULL)
		{
			bRet = FALSE;
		}

		char szBuf[ATLSMTP_MAX_LINE_LENGTH+1];
		int nBufLen = ATLSMTP_MAX_LINE_LENGTH;
		if (bRet)
		{
			// See if the connect returns success
			AtlSmtpReadData((HANDLE)m_hSocket, szBuf, &nBufLen, &m_Overlapped);
			if (strncmp(szBuf, ATLSMTP_CONN_SUCC, ATLSMTP_RETCODE_LEN))
			{
				bRet = FALSE;
			}
		}

		char szLocalHost[ATLSMTP_MAX_SERVER_NAME_LENGTH+1];

		// gethostname should return 0 on success
		if (bRet && gethostname(szLocalHost, ATLSMTP_MAX_SERVER_NAME_LENGTH))
		{
			bRet = FALSE;
		}

		// Send HELO command and get reply
		if (bRet)
		{
			nBufLen = sprintf(szBuf, "HELO %s\r\n", szLocalHost);
			bRet = AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, szBuf, &nBufLen, 
										ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_HELO_SUCC, &m_Overlapped);
		}

		if (!bRet)
		{
			if (m_Overlapped.hEvent != NULL)
				CloseHandle(m_Overlapped.hEvent);
			shutdown(m_hSocket, SD_BOTH);
			closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
		}

		return bRet;
	}

	// Disconnect the socket
	inline BOOL Disconnect() throw()
	{
		if (!Connected())
		{
			return FALSE;
		}

		// shutdown should return 0 on success
		if (shutdown(m_hSocket, SD_BOTH))
		{
			return FALSE;
		}

		// closesocket should return 0 on success
		if (closesocket(m_hSocket))
		{
			return FALSE;
		}

		// close the handle to the overlapped event
		CloseHandle(m_Overlapped.hEvent);
		m_hSocket = INVALID_SOCKET;
		memset((void*)&m_Overlapped, 0, sizeof(OVERLAPPED));
		return TRUE;
	}

	// Are we connected?
	inline BOOL Connected() throw()
	{
		return (m_hSocket != INVALID_SOCKET ? TRUE : FALSE);
	}

	// Send a message from a file
	// lpszFileName - the file containing the message
	// lpszRecipients - the recipients to send to (optional - if not specified, the recipients specified
	//		in the file will be used
	// lpszSender - the sender (optional - if not specified, the recipients specified in the file
	//		will be used
	BOOL SendMessage(LPCTSTR lpszFileName, LPCTSTR lpszRecipients = NULL, LPCTSTR lpszSender = NULL) throw()
	{
		if (!Connected())
		{
			return FALSE;
		}

		//Try to open the file
		CAtlFile readFile;
		if (FAILED(readFile.Create(lpszFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL)))
		{
			return FALSE;
		}

		char szBuf[ATLSMTP_MAX_LINE_LENGTH+1];
		int nBufLen = ATLSMTP_MAX_LINE_LENGTH;
		BOOL bDumpedSender = FALSE;

		//If the caller specifies the sender, rather than having an existing one in the file...
		if (lpszSender)
		{
			nBufLen = sprintf(szBuf, "MAIL FROM:<%s>\r\n", CT2CA(lpszSender));
			if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, 
									ATLSMTP_MAIL_SUCC, &m_Overlapped))
			{
				return FALSE;
			}
			bDumpedSender = TRUE;
		}
		nBufLen = ATLSMTP_MAX_LINE_LENGTH;

#ifdef ATLSMTP_DOUBLE_BUFFERED
		char buffer1[ATLSMTP_READBUFFER_SIZE];
		char buffer2[ATLSMTP_READBUFFER_SIZE];
		char* currBuffer = buffer1;
		char* prevBuffer = NULL;

		int nCurrBuffer = 0;
		DWORD dwPrevLength = 0;
		DWORD dwWritten = 0;
#else
		char bakBuffer[ATLSMTP_READBUFFER_SIZE];
		char* currBuffer = bakBuffer;

#endif // ATLSMTP_DOUBLE_BUFFERED
		DWORD dwRead = 0;
		DWORD dwBytesInBuffer = 0;
		DWORD dwBufPos = 0;

		//first handle the MAIL FROM and RCPT TO commands
		BOOL bDumpedRecipients = FALSE;
		BOOL bRet = TRUE;
		while (bRet)
		{
			int nRetCode = 0;

			//if we have dumped the sender, and we have extra recipients to send,
			//and we haven't alredy done so, do it
			if (lpszRecipients && !bDumpedRecipients && bDumpedSender)
			{
				bRet = DumpRecipients((HANDLE)m_hSocket, CT2A(lpszRecipients), &m_Overlapped, ATLSMTP_FOR_SEND);
			}

			if (bRet)
			{
				dwRead = 0;
				BOOL bFullLine = FALSE;
				bRet = ReadLine(readFile, currBuffer, szBuf, &dwBytesInBuffer, &dwBufPos,
					ATLSMTP_READBUFFER_SIZE, ATLSMTP_MAX_LINE_LENGTH, &dwRead, &bFullLine);
				if (dwRead == 0 || bFullLine == FALSE)
					bRet = FALSE;
			}

			if (bRet)
			{
				bRet = AtlSmtpSendAndWait((HANDLE)m_hSocket, szBuf, (int)(dwRead), &m_Overlapped);
			}

			if (bRet)
			{
				nBufLen = ATLSMTP_MAX_LINE_LENGTH;
				bRet = AtlSmtpReadData((HANDLE)m_hSocket, szBuf, &nBufLen, &m_Overlapped);
			}

			if (bRet)
			{	
				nRetCode = atoi(szBuf);
				//if the command is equal to ATLSMTP_MAIL_SUCC (or RCPT_SUCC: they are equivalent)
				if (nRetCode == ATLSMTP_MAIL_SUCCESS || nRetCode == ATLSMTP_RCPT_NOT_LOCAL || nRetCode == ATLSMTP_RCPT_SUCCESS)
				{
					bDumpedSender = TRUE;
					continue;
				}

				//If the command is equal to the data intermediate success code,
				//break out of the loop
				if (nRetCode == ATLSMTP_DATA_INTERMEDIATE)
					break;
			}

			//otherwise, we got an error code
			CancelMessage();
			return FALSE;
		}

		dwRead = dwBytesInBuffer;
		currBuffer+= dwBufPos;
		DWORD dwErr = 0;
		do
		{
			dwErr = 0;

			//Try to send the data
#ifdef ATLSMTP_DOUBLE_BUFFERED
			if (!AtlSmtpSendOverlapped((HANDLE)m_hSocket, currBuffer, dwRead, prevBuffer, dwPrevLength, &m_Overlapped))
			{
				bRet = FALSE;
				break;
			}
#else
			if (!AtlSmtpSendAndWait((HANDLE)m_hSocket, currBuffer, dwRead, &m_Overlapped))
			{
				bRet = FALSE;
				break;
			}
#endif // ATLSMTP_DOUBLE_BUFFERED

			//swap the current and previous buffer
#ifdef ATLSMTP_DOUBLE_BUFFERED
			prevBuffer = currBuffer;
			currBuffer = (nCurrBuffer == 0 ? buffer2 : buffer1);
			nCurrBuffer = (nCurrBuffer == 0 ? 1 : 0);
			dwPrevLength = dwBytesInBuffer;
#else
			currBuffer = bakBuffer;
#endif // ATLSMTP_DOUBLE_BUFFERED

			if (FAILED(readFile.Read(currBuffer, ATLSMTP_READBUFFER_SIZE, dwRead)))
			{
				bRet = FALSE;
				break;
			}
		} while (dwRead != 0);

		//ensure that the last of the data is sent
#ifdef ATLSMTP_DOUBLE_BUFFERED
		if (!GetOverlappedResult((HANDLE)m_hSocket, &m_Overlapped, &dwWritten, TRUE))
		{
			if ((dwErr = GetLastError()) != ERROR_IO_PENDING && dwErr != ERROR_IO_INCOMPLETE)
				bRet = FALSE;
			else if (dwWritten < dwPrevLength)
				bRet = AtlSmtpSendAndWait((HANDLE)m_hSocket, prevBuffer+dwWritten, dwPrevLength-dwWritten, &m_Overlapped);
		}
#endif // ATLSMTP_DOUBLE_BUFFERED


		if (bRet)
		{
			// End the message with a CRLF.CRLF
			nBufLen = sprintf(szBuf, "\r\n.\r\n");
			if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, 
				szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_DATA_SUCC, &m_Overlapped))
			{
				bRet = FALSE;
			}
		}

		return bRet;
	}

	// Send the message
	// msg - the CMimeMessage to send
	// lpszSender - the sender 
	inline BOOL SendMessage(CMimeMessage& msg, LPCTSTR lpszRecipients = NULL, LPCTSTR lpszSender = NULL) throw()
	{
		if (!Connected())
		{
			return FALSE;
		}

		char szBuf[ATLSMTP_MAX_LINE_LENGTH+1];

		//Send MAIL FROM command and get reply
		int nBufLen = sprintf(szBuf, "MAIL FROM:<%s>\r\n", 
			(lpszSender ? CT2CA(lpszSender) : msg.GetSender()));
		if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, 
				szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_MAIL_SUCC, &m_Overlapped))
		{
			return FALSE;
		}

		BOOL bRet = TRUE;
		if (!lpszRecipients)
		{
			LPSTR lpszRecipientsA = NULL;
			DWORD dwLen = msg.GetRequiredRecipientsStringLength();
			lpszRecipientsA = static_cast<LPSTR>(malloc(sizeof(char)*dwLen));
			if (!lpszRecipientsA || msg.GetRecipientsString(lpszRecipientsA, &dwLen) == FALSE)
			{
				bRet = FALSE;
			}
			if (bRet)
				bRet = DumpRecipients((HANDLE)m_hSocket, lpszRecipientsA, &m_Overlapped, ATLSMTP_FOR_SEND);
			free(lpszRecipientsA);
		}
		else
		{
			bRet = DumpRecipients((HANDLE)m_hSocket, CT2CA(lpszRecipients), 
						&m_Overlapped, ATLSMTP_FOR_SEND);
		}

		//Begin the data output
		if (bRet)
		{
			nBufLen = sprintf(szBuf, "DATA\r\n");
			bRet = AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, 
						szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_DATA_INTM, &m_Overlapped);
		}

		if (!bRet)
			CancelMessage();

		//Attempt to write the data to the socket
		if (bRet)
		{
			bRet = msg.DumpData((HANDLE)m_hSocket, &m_Overlapped, NULL, ATLSMTP_FORMAT_SMTP);
		}

		if (bRet)
		{
			//End the message with a <CRLF>.<CRLF>
			nBufLen = sprintf(szBuf, "\r\n.\r\n");
			if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, 
					szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_DATA_SUCC, &m_Overlapped))
			{
				return FALSE;
			}
		}

		return bRet;
	}

	// Send a chunk of raw data
	inline BOOL SendRaw(LPCTSTR lpszRawData, DWORD dwLen, LPCTSTR lpszRecipients, LPCTSTR lpszSender) throw()
	{
		ATLASSERT(lpszRawData != NULL);
		ATLASSERT(lpszRecipients != NULL);
		ATLASSERT(lpszSender != NULL);

		if (!Connected())
			return FALSE;

		char szBuf[ATLSMTP_MAX_LINE_LENGTH+1];

		//Send MAIL FROM command and get reply
		int nBufLen = sprintf(szBuf, "MAIL FROM:<%s>\r\n", CT2CA(lpszSender));
		if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, 
				szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_MAIL_SUCC, &m_Overlapped))
		{
			return FALSE;
		}

		BOOL bRet = DumpRecipients((HANDLE)m_hSocket, CT2CA(lpszRecipients),
						&m_Overlapped, ATLSMTP_FOR_SEND);

		// Begin the data output
		if (bRet)
		{
			nBufLen = sprintf(szBuf, "DATA\r\n");
			bRet = AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen,
						szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_DATA_INTM, &m_Overlapped);
		}

		if (!bRet)
			CancelMessage();

		if (bRet)
		{
			bRet = AtlSmtpSendAndWait((HANDLE)m_hSocket, (LPSTR)(lpszRawData), dwLen, &m_Overlapped);
		}

		if (bRet)
		{
			//End the message with a <CRLF>.<CRLF>
			nBufLen = sprintf(szBuf, "\r\n.\r\n");
			if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, 
					szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, ATLSMTP_DATA_SUCC, &m_Overlapped))
			{
				return FALSE;
			}
		}

		return bRet;
	}

	inline BOOL SendSimple(LPCTSTR lpszRecipients, LPCTSTR lpszSender, LPCTSTR lpszSubject, LPCTSTR lpszBody, int nTextLen = -1) throw()
	{
		CMimeMessage msg;
		BOOL bRet = msg.SetSubject(lpszSubject);
		if (bRet)
			bRet = msg.AddText(lpszBody, nTextLen);
		if (bRet)
			bRet = SendMessage(msg, lpszRecipients, lpszSender);
		return bRet;
	}

	// Save a MIME message to a file
	// lpszFileName - the file name
	// lpszRecipients - the recipients string (optional)
	// lpszSender - the sender (optional)
	// dwFlags - the flags (optional)
	inline BOOL WriteToFile(LPCTSTR lpszFileName, CMimeMessage& msg, LPCTSTR lpszRecipients = NULL, 
		LPCTSTR lpszSender = NULL, DWORD dwFlags = 0) throw()
	{
		//Try to create/open the file
		HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		// Use CHandle to close the file handle
		// (CAtlFile does not allow for overlapped I/O)
		CHandle hdlFile(hFile);

		//Create and initialize the OVERLAPPED struct
		OVERLAPPED writeOverlapped;
		memset((void*)&writeOverlapped, 0, sizeof(OVERLAPPED));
		writeOverlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		if (writeOverlapped.hEvent == NULL)
		{
			return FALSE;
		}

		// Use CHandle to close the event handle
		CHandle hdlEvent(writeOverlapped.hEvent);

		char szBuf[ATLSMTP_MAX_LINE_LENGTH+1];
		BOOL bRet = TRUE;

		int nBufLen = 0;

		//if writing to file for purposes of sending, write out the
		//commands as well
		if (lpszSender || (dwFlags & ATLSMTP_DUMP_SENDER))
		{
			nBufLen = sprintf(szBuf, "MAIL FROM:<%s>\r\n", 
				(lpszSender ? CT2CA(lpszSender) : msg.GetSender()));
			bRet = AtlSmtpSendAndWait(hFile, szBuf, nBufLen, &writeOverlapped);
		}

		if (bRet && (lpszRecipients || (dwFlags & ATLSMTP_DUMP_RECIPS)))
		{
			if (!lpszRecipients)
			{
				LPSTR lpszRecipientsA = NULL;
				DWORD dwLen = msg.GetRequiredRecipientsStringLength();
				lpszRecipientsA = static_cast<LPSTR>(malloc(sizeof(char)*dwLen));
				if (!lpszRecipientsA || msg.GetRecipientsString(lpszRecipientsA, &dwLen) == FALSE)
				{
					bRet = FALSE;
				}
				if (bRet)
					bRet = DumpRecipients(hFile, lpszRecipientsA, &writeOverlapped);
				free(lpszRecipientsA);
			}
			else
			{
				bRet = DumpRecipients(hFile, CT2CA(lpszRecipients), &writeOverlapped);
			}
		}

		if (bRet)
		{
			nBufLen = sprintf(szBuf, "DATA\r\n");
			bRet = AtlSmtpSendAndWait(hFile, szBuf, nBufLen, &writeOverlapped);
		}

		if (bRet)
		{
			bRet = msg.DumpData(hFile, &writeOverlapped, NULL, ATLSMTP_FORMAT_SMTP);
		}

		return bRet;
	}

protected:

	// disallow copy construction
	CSMTPConnection(const CSMTPConnection&) throw()
	{
		ATLASSERT(FALSE);
	}

	// disallow assignment
	const CSMTPConnection& operator=(const CSMTPConnection&) throw()
	{
		ATLASSERT(FALSE);
		return *this;
	}

	// Tell the server we are aborting the message
	inline BOOL CancelMessage() throw()
	{
		char szBuf[ATLSMTP_MAX_LINE_LENGTH+1];
		int nBufLen = sprintf(szBuf, "RSET\r\n");
		if (!AtlSmtpSendAndCheck((HANDLE)m_hSocket, szBuf, nBufLen, szBuf, &nBufLen, ATLSMTP_MAX_LINE_LENGTH, 
			ATLSMTP_RSET_SUCC, &m_Overlapped))
		{
			Disconnect();
			return FALSE;
		}
		return TRUE;
	}

	// Dump the recipients to hFile
	// lpszRecipients - the recipients string
	// pOverlapped - the OVERALAPPED struct
	// dwFlags - the flags
	inline BOOL DumpRecipients(HANDLE hFile, LPCSTR lpszRecipients, LPOVERLAPPED pOverlapped, DWORD dwFlags = 0) throw()
	{
		ATLASSERT(lpszRecipients != NULL);
		ATLASSERT(pOverlapped != NULL);

		char  rcptBuf[ATLSMTP_MAX_LINE_LENGTH-12];
		char  szBuf[ATLSMTP_MAX_LINE_LENGTH];
		LPSTR tmpBuf = rcptBuf;
		char ch;
		BOOL bRet = TRUE;
		int nMaxLength = ATLSMTP_MAX_LINE_LENGTH;
		int nRetCode = 0;
		do
		{
			ch = *lpszRecipients;
			if (ch)
				lpszRecipients++;
			if (AtlSmtpIsRecipientDelimiter(ch))
			{
				*tmpBuf = 0;
				int nBufLen = sprintf(szBuf, "RCPT TO:<%s>\r\n", rcptBuf);
				bRet = AtlSmtpSendAndWait(hFile, szBuf, nBufLen, pOverlapped);
				if (bRet && (dwFlags & ATLSMTP_FOR_SEND))
				{
					bRet = AtlSmtpReadData(hFile, szBuf, &nMaxLength, pOverlapped);
					nRetCode = atoi(szBuf);
					if (!bRet || (nRetCode != ATLSMTP_RCPT_SUCCESS && nRetCode != ATLSMTP_RCPT_NOT_LOCAL))
					{
						bRet = FALSE;
						break;
					}
				}
				tmpBuf = rcptBuf;
				nMaxLength = ATLSMTP_MAX_LINE_LENGTH;
				while (isspace(*lpszRecipients))
					lpszRecipients++;
				continue;
			}
			
			*tmpBuf++ = ch;
		} while (ch != '\0');
		
		return bRet;
	}

	// Implementation - used from ReadLine
	// fills pBuf with up to dwMaxLen bytes
	BOOL FillBuffer(HANDLE hFile, LPSTR pBuf, DWORD dwMaxLen, LPDWORD pdwLen) throw()
	{
		ATLASSERT(hFile != INVALID_HANDLE_VALUE);
		ATLASSERT(pdwLen != NULL);

		DWORD dwRead = 0;
		DWORD dwTotalRead = 0;
		int nRet = 0;

		do 
		{
			nRet = ReadFile(hFile, pBuf, dwMaxLen-dwTotalRead, &dwRead, NULL);
			if (!nRet && GetLastError() != ERROR_HANDLE_EOF)
			{
				return FALSE;
			}

			if (dwRead == 0)
				break;

			dwTotalRead+= dwRead;
		} while (dwTotalRead < dwMaxLen);

		*pdwLen = dwTotalRead;

		return TRUE;
	}

	// Implementation
	// Read a line (terminated by LF) from hFile
	BOOL ReadLine(HANDLE hFile, LPSTR pSrc, LPSTR pDest, LPDWORD pdwSrcLen, LPDWORD pdwBufPos, DWORD dwMaxSrcLen, 
			DWORD dwMaxDestLen, LPDWORD pdwRead=NULL, LPBOOL pbFullLine=NULL) throw()
	{
		ATLASSERT(hFile != INVALID_HANDLE_VALUE);
		ATLASSERT(pSrc != NULL);
		ATLASSERT(pDest != NULL);
		ATLASSERT(pdwSrcLen != NULL);
		ATLASSERT(pdwBufPos != NULL);

		BOOL bRet = TRUE;
		DWORD dwLen = 0;
		DWORD dwBufPos = 0;
		DWORD dwSrcLen = *pdwSrcLen;
		LPSTR pSrcCurrent = pSrc + *pdwBufPos;

		while (bRet && dwLen < dwMaxDestLen)
		{
			if (dwSrcLen == 0)
			{
				if (!FillBuffer(hFile, pSrc, dwMaxSrcLen, pdwSrcLen) || *pdwSrcLen == 0)
					break;

				dwBufPos = 0;
				*pdwBufPos = 0;
				dwSrcLen = *pdwSrcLen;
				pSrcCurrent = pSrc;
			}

			--dwSrcLen;
			*pDest = *pSrcCurrent++;
			dwLen++;
			dwBufPos++;
			if (*pDest == '\n')
			{
				break;
			}
			pDest++;
		}

		*pdwSrcLen = dwSrcLen;

		if (pbFullLine)
		{
			if (*pDest != '\n')
				*pbFullLine = FALSE;
			else
				*pbFullLine = TRUE;
		}

		if (pdwRead)
			*pdwRead = dwLen;

		*pdwBufPos += dwBufPos;

		return bRet;
	}

}; // class CSMTPConnection

} // namespace ATL