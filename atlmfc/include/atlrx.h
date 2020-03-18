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

#include <atlbase.h>
#include <atlcoll.h>
#include <mbstring.h>

#ifndef ATL_REGEXP_MIN_STACK
#define ATL_REGEXP_MIN_STACK 256
#endif

namespace ATL {

template <class CharTraits=CAtlRECharTraitsA>
class CAtlREMatchContext
{
public:
	typedef CharTraits::RECHARTYPE RECHAR;
	struct MatchGroup
	{
		const RECHAR *szStart;
		const RECHAR *szEnd;
	};

	MatchGroup *m_pMatches;
	int m_nNumGroups;

	CAtlArray<void *> m_stack;
	int m_nTos;

	void **m_pMem;
	int m_nAllocedMem;

	MatchGroup m_Match;

	CAtlREMatchContext()
	{
		m_pMatches = NULL;
		m_nNumGroups = 0;
		m_nTos = 0;
		m_nAllocedMem = 0;
		m_pMem = NULL;
		m_stack.SetSize(ATL_REGEXP_MIN_STACK);
		m_Match.szStart = NULL;
		m_Match.szEnd = NULL;
	}

	~CAtlREMatchContext()
	{
		delete [] m_pMem;
		delete [] m_pMatches;
	}
	BOOL Initialize(int nRequiredMem, int nNumGroups)
	{
		m_nNumGroups = nNumGroups;

		m_nTos = 0;

		if (m_pMatches)
			delete [] m_pMatches;
		m_pMatches = new MatchGroup[nNumGroups];
		if (!m_pMatches)
			return FALSE;

		if (m_pMem)
			delete [] m_pMem;
		m_pMem = new void* [nRequiredMem];
		if (!m_pMem)
			return FALSE;
		m_nAllocedMem = nRequiredMem;
		memset(m_pMatches, 0x00, m_nNumGroups * sizeof(MatchGroup));
		return TRUE;
	}

	BOOL Push(void *p)
	{
		m_nTos++;
		if (m_stack.GetCount() <= (UINT) m_nTos)
		{
			if (!m_stack.SetSize((m_nTos+1)*2))
				return FALSE;
		}
		m_stack[m_nTos] = p;
		return TRUE;
	}

	BOOL Push(int n)
	{
		return Push((void *) n);
	}
	void *Pop()
	{
		if (m_nTos==0)
			return NULL;
		void *p = m_stack[m_nTos];
		m_nTos--;
		return p;
	}
};

class CAtlRECharTraitsA
{
public:
	typedef char RECHARTYPE;

	static RECHARTYPE *Next(const RECHARTYPE *sz)
	{
		return (RECHARTYPE *) (sz+1);
	}

	static int Strncmp(const RECHARTYPE *szLeft, const RECHARTYPE *szRight, int nCount)
	{
		return strncmp(szLeft, szRight, nCount);
	}

	static int Strnicmp(const RECHARTYPE *szLeft, const RECHARTYPE *szRight, int nCount)
	{
		return strnicmp(szLeft, szRight, nCount);
	}

	static RECHARTYPE *Strlwr(RECHARTYPE *sz)
	{
		return _strlwr(sz);
	}

	static long Strtol(const RECHARTYPE *sz, RECHARTYPE **szEnd, int nBase)
	{
		return strtol(sz, szEnd, nBase);
	}

	static int Isdigit(RECHARTYPE ch)
	{
		return isdigit(ch);
	}

	static const RECHARTYPE** GetAbbrevs()
	{
		static const RECHARTYPE *s_szAbbrevs[] = 
		{
			"a[a-zA-Z0-9]",	// alpha numeric
			"b[ \\t]*",		// white space (blank)
			"c[a-zA-Z]",	// alpha
			"d[0-9]",		// digit
			"h[0-9a-fA-F]",	// hex digit
			"n(\r|(\r?\n))",	// newline
			"q(\"[^\"]*\")|(\'[^\']*\')",	// quoted string
			"w[a-zA-Z]+",	// simple word
			"z[0-9]+",		// integer
			NULL
		};

		return s_szAbbrevs;
	}

	static BOOL UseBitFieldForRange()
	{
		return TRUE;
	}

	static int Strlen(const RECHARTYPE *sz)
	{
		return strlen(sz);
	}
};

class CAtlRECharTraitsW
{
public:
	typedef WCHAR RECHARTYPE;

	static RECHARTYPE *Next(const RECHARTYPE *sz)
	{
		return (RECHARTYPE *) (sz+1);
	}

	static int Strncmp(const RECHARTYPE *szLeft, const RECHARTYPE *szRight, int nCount)
	{
		return wcsncmp(szLeft, szRight, nCount);
	}

	static int Strnicmp(const RECHARTYPE *szLeft, const RECHARTYPE *szRight, int nCount)
	{
		return _wcsnicmp(szLeft, szRight, nCount);
	}

	static RECHARTYPE *Strlwr(RECHARTYPE *sz)
	{
		return _wcslwr(sz);
	}

	static long Strtol(const RECHARTYPE *sz, RECHARTYPE **szEnd, int nBase)
	{
		return wcstol(sz, szEnd, nBase);
	}

	static int Isdigit(RECHARTYPE ch)
	{
		return iswdigit(ch);
	}

	static const RECHARTYPE** GetAbbrevs()
	{
		static const RECHARTYPE *s_szAbbrevs[] = 
		{
			L"a[a-zA-Z0-9]",	// alpha numeric
			L"b[ \\t]*",		// white space (blank)
			L"c[a-zA-Z]",	// alpha
			L"d[0-9]",		// digit
			L"h[0-9a-fA-F]",	// hex digit
			L"n(\r|(\r?\n))",	// newline
			L"q(\"[^\"]*\")|(\'[^\']*\')",	// quoted string
			L"w[a-zA-Z]+",	// simple word
			L"z[0-9]+",		// integer
			NULL
		};

		return s_szAbbrevs;
	}

	static BOOL UseBitFieldForRange()
	{
		return FALSE;
	}

	static int Strlen(const RECHARTYPE *sz)
	{
		return wcslen(sz);
	}
};

class CAtlRECharTraitsMB
{
public:
	typedef unsigned char RECHARTYPE;

	static RECHARTYPE *Next(const RECHARTYPE *sz)
	{
		return _mbsinc(sz);
	}

	static int Strncmp(const RECHARTYPE *szLeft, const RECHARTYPE *szRight, int nCount)
	{
		return _mbsncmp(szLeft, szRight, nCount);
	}

	static int Strnicmp(const RECHARTYPE *szLeft, const RECHARTYPE *szRight, int nCount)
	{
		return _mbsnicmp(szLeft, szRight, nCount);
	}

	static RECHARTYPE *Strlwr(RECHARTYPE *sz)
	{
		return _mbslwr(sz);
	}

	static long Strtol(const RECHARTYPE *sz, RECHARTYPE **szEnd, int nBase)
	{
		return strtol((const char *) sz, (char **) szEnd, nBase);
	}

	static int Isdigit(RECHARTYPE ch)
	{
		return _ismbcdigit((unsigned int) ch);
	}

	static const RECHARTYPE** GetAbbrevs()
	{
		static const char *s_szAbbrevs[] = 
		{
			"a[a-zA-Z0-9]",	// alpha numeric
			"b[ \\t]*",		// white space (blank)
			"c[a-zA-Z]",	// alpha
			"d[0-9]",		// digit
			"h[0-9a-fA-F]",	// hex digit
			"n(\r|(\r?\n))",	// newline
			"q(\"[^\"]*\")|(\'[^\']*\')",	// quoted string
			"w[a-zA-Z]+",	// simple word
			"z[0-9]+",		// integer
			NULL
		};

		return (const RECHARTYPE **) s_szAbbrevs;
	}

	static BOOL UseBitFieldForRange()
	{
		return FALSE;
	}

	static int Strlen(const RECHARTYPE *sz)
	{
		return strlen((const char *) sz);
	}
};


template <class CharTraits=CAtlRECharTraitsA>
class CAtlRegExp
{
public:
	typedef CharTraits::RECHARTYPE RECHAR;

	enum REInstructionType { 
		RE_NOP,
		RE_GROUP_START,
		RE_GROUP_END, 
		RE_GROUP_RESET,
		RE_SYMBOL,
		RE_ANY,
		RE_RANGE,
		RE_NOTRANGE,
		RE_RANGE_EX,
		RE_NOTRANGE_EX,
		RE_PLUS,
		RE_NG_PLUS,
		RE_QUESTION,
		RE_NG_QUESTION,
		RE_JMP,
		RE_PUSH_CHARPOS,
		RE_POP_CHARPOS,
		RE_CALL,
		RE_RETURN,
		RE_STAR_BEGIN,
		RE_NG_STAR_BEGIN, 
		RE_PUSH_MEMORY,
		RE_POP_MEMORY,
		RE_STORE_CHARPOS,
		RE_STORE_STACKPOS,
		RE_GET_CHARPOS,
		RE_GET_STACKPOS,
		RE_RET_NOMATCH,
		RE_PREVIOUS,
		RE_FAIL,
		RE_MATCH,
	};

	struct INSTRUCTION_SYMBOL
	{
		int nSymbol;
	};

	struct INSTRUCTION_JMP
	{
		int nTarget;	
	};

	struct INSTRUCTION_GROUP
	{
		int nGroup;
	};

	struct INSTRUCTION_CALL
	{
		int nTarget;
	};

	struct INSTRUCTION_MEMORY
	{
		int nIndex;
	};

	struct INSTRUCTION_PREVIOUS
	{
		int nGroup;
	};

	struct INSTRUCTION_RANGE_EX
	{
		int nTarget;
	};

	struct INSTRUCTION
	{
		REInstructionType type;
		union
		{
			INSTRUCTION_SYMBOL symbol;
			INSTRUCTION_JMP jmp;
			INSTRUCTION_GROUP group;
			INSTRUCTION_CALL call;
			INSTRUCTION_MEMORY memory;
			INSTRUCTION_PREVIOUS prev;
			INSTRUCTION_RANGE_EX range;
		};
	};

	CAtlArray<INSTRUCTION> m_Instructions;

	int m_nNumGroups;
	int m_nRequiredMem;
	BOOL m_bCaseSensitive;

	CAtlRegExp()
	{
		m_nNumGroups = 0;
		m_nRequiredMem = 0;
		m_bCaseSensitive = TRUE;
	}

	// class used internally to restore
	// parsing state when unwinding
	class CParseState
	{
	public:
		int m_nNumInstructions;
		int m_nNumGroups;
		int m_nRequiredMem;

		CParseState(CAtlRegExp *pRegExp)
		{
			m_nNumInstructions = (int) pRegExp->m_Instructions.GetCount();
			m_nNumGroups = pRegExp->m_nNumGroups;
			m_nRequiredMem = pRegExp->m_nRequiredMem;
		}

		void Restore(CAtlRegExp *pRegExp)
		{
			pRegExp->m_Instructions.SetSize(m_nNumInstructions);
			pRegExp->m_nNumGroups = m_nNumGroups;
			pRegExp->m_nRequiredMem = m_nRequiredMem;
		}
	};

	int AddInstruction(REInstructionType type)
	{
		if (!m_Instructions.SetSize(m_Instructions.GetCount()+1))
			return NULL;

		m_Instructions[m_Instructions.GetCount()-1].type = type;
		return (int) m_Instructions.GetCount()-1;
	}

	BOOL PeekToken(const RECHAR **ppszRE, int ch)
	{
		if (**ppszRE != ch)
			return FALSE;
		return TRUE;
	}

	BOOL MatchToken(const RECHAR **ppszRE, int ch)
	{
		if (!PeekToken(ppszRE, ch))
			return FALSE;
		*ppszRE = CharTraits::Next(*ppszRE);
		return TRUE;
	}

	INSTRUCTION &GetInstruction(int nIndex)
	{
		return m_Instructions[nIndex];
	}
	int ParseArg(const RECHAR **ppszRE)
	{
		int p = AddInstruction(RE_GROUP_START);
		GetInstruction(p).group.nGroup = m_nNumGroups++;

		int nCall = AddInstruction(RE_CALL);
		int nReset = AddInstruction(RE_GROUP_RESET);
		AddInstruction(RE_RETURN);

		int nAlt = ParseRE(ppszRE);
		if (nAlt < 0)
		{
			if (!PeekToken(ppszRE, '}'))
				return -1;

			// in the case of an empty group, we add a nop
			nAlt = AddInstruction(RE_NOP);
		}

		GetInstruction(nCall).call.nTarget = nAlt;
		GetInstruction(nReset).group.nGroup = GetInstruction(p).group.nGroup;

		if (!MatchToken(ppszRE, '}'))
			return -1;

		int nEnd = AddInstruction(RE_GROUP_END);
		GetInstruction(nEnd).group.nGroup = GetInstruction(p).group.nGroup;
		return p;
	}

	int ParseGroup(const RECHAR **ppszRE)
	{
		int nCall = AddInstruction(RE_CALL);
		AddInstruction(RE_RETURN);

		int nAlt = ParseRE(ppszRE);
		if (nAlt < 0)
		{
			if (!PeekToken(ppszRE, ')'))
				return -1;

			// in the case of an empty group, we add a nop
			nAlt = AddInstruction(RE_NOP);
		}

		GetInstruction(nCall).call.nTarget = nAlt;

		if (!MatchToken(ppszRE, ')'))
			return -1;

		return nCall;
	}

	RECHAR GetEscapedChar(RECHAR ch)
	{
		if (ch == 't')
			return '\t';
		return ch;
	}

	int ParseCharItem(const RECHAR **ppszRE, RECHAR *pchStartChar, RECHAR *pchEndChar)
	{
		if (**ppszRE == '\\')
		{
			*ppszRE = CharTraits::Next(*ppszRE);
			*pchStartChar = GetEscapedChar(**ppszRE);
		}
		else
			*pchStartChar = **ppszRE;
		*ppszRE = CharTraits::Next(*ppszRE);

		if (!MatchToken(ppszRE, '-'))
		{
			*pchEndChar = *pchStartChar;
			return 0;
		}

		// check for unterminated range
		if (!**ppszRE || PeekToken(ppszRE, ']'))
			return -1;

		*pchEndChar = **ppszRE;
		*ppszRE = CharTraits::Next(*ppszRE);

		return 0;
	}

	void AddInstructions(int nNumInstructions)
	{
		size_t nCurr = m_Instructions.GetCount();
		m_Instructions.SetSize(nCurr+nNumInstructions);
	}

	int ParseCharSet(const RECHAR **ppszRE, BOOL bNot)
	{
		int p = -1;
		
		unsigned char *pBits = NULL;
		
		if (CharTraits::UseBitFieldForRange())
		{
			// we use a bit field to represent the characters
			// a 1 bit means match against the character
			// the last 5 bits are used as an index into 
			// the byte array, and the first 3 bits
			// are used to index into the selected byte

			p = AddInstruction(bNot ? RE_NOTRANGE : RE_RANGE);
			if (p < 0)
				return p;

			// add the required space to hold the character
			// set.  We use one bit per character for ansi
			// review: unicode is not currently supported
			AddInstructions((256/8) / sizeof(INSTRUCTION) + ((256/8) % sizeof(INSTRUCTION) ? 1 : 0));


			pBits = (unsigned char *) (&m_Instructions[p+1]);
			memset(pBits, 0x00, 256/8);
		}
		else
		{
			p = AddInstruction(bNot ? RE_NOTRANGE_EX : RE_RANGE_EX);
		}

		RECHAR chStart;
		RECHAR chEnd;

		while (**ppszRE && **ppszRE != ']')
		{
			if (ParseCharItem(ppszRE, &chStart, &chEnd))
				return -1;

			if (CharTraits::UseBitFieldForRange())
			{
				for (int i=chStart; i<=chEnd; i++)
					pBits[i >> 3] |= 1 << (i & 0x7);
			}
			else
			{
				int nStart = AddInstruction(RE_NOP);
				int nEnd = AddInstruction(RE_NOP);

				GetInstruction(nStart).memory.nIndex = (int) chStart;
				GetInstruction(nEnd).memory.nIndex = (int) chEnd;
			}
		}

		if (!CharTraits::UseBitFieldForRange())
			GetInstruction(p).range.nTarget = m_Instructions.GetCount();

		return p;
	}

	int ParseCharClass(const RECHAR **ppszRE)
	{
		if (MatchToken(ppszRE, ']'))
			return -1;

		BOOL bNot = FALSE;
		if (MatchToken(ppszRE, '^'))
			bNot = TRUE;

		if (MatchToken(ppszRE, ']'))
			return -1;

		int p = ParseCharSet(ppszRE, bNot);
		if (p < 0)
			return p;
		if (!MatchToken(ppszRE, ']'))
			return -1;
		return p;
	}

	int AddMemInstruction(REInstructionType type)
	{
		int p = AddInstruction(type);
		if (p < 0)
			return p;
		GetInstruction(p).memory.nIndex = m_nRequiredMem++;
		return p;
	}

	int ParseNot(const RECHAR **ppszRE)
	{
		int nStoreCP = AddMemInstruction(RE_STORE_CHARPOS);
		int nStoreSP = AddMemInstruction(RE_STORE_STACKPOS);

		int nCall = AddInstruction(RE_CALL);

		int nGetCP = AddInstruction(RE_GET_CHARPOS);
		GetInstruction(nGetCP).memory.nIndex = GetInstruction(nStoreCP).memory.nIndex;

		int nGetSP = AddInstruction(RE_GET_STACKPOS);
		GetInstruction(nGetSP).memory.nIndex = GetInstruction(nStoreSP).memory.nIndex;

		int nJmp = AddInstruction(RE_JMP);
		
		int nSE = ParseSE(ppszRE);
		if (nSE < 0)
			return nSE;

		// patch the call
		GetInstruction(nCall).call.nTarget = nSE;

		int nGetCP1 = AddInstruction(RE_GET_CHARPOS);
		GetInstruction(nGetCP1).memory.nIndex = GetInstruction(nStoreCP).memory.nIndex;

		int nGetSP1 = AddInstruction(RE_GET_STACKPOS);
		GetInstruction(nGetSP1).memory.nIndex = GetInstruction(nStoreSP).memory.nIndex;

		int nRet = AddInstruction(RE_RETURN);

		GetInstruction(nJmp).jmp.nTarget = nRet+1;

		return nStoreCP;
	}

	int ParseAbbrev(const RECHAR **ppszRE)
	{
		const RECHAR **szAbbrevs = CharTraits::GetAbbrevs();

		while (*szAbbrevs)
		{
			if (**ppszRE == **szAbbrevs)
			{
				const RECHAR *szAbbrev = (*szAbbrevs)+1;
				int p = ParseE(&szAbbrev);
				if (p < 0)
					return p;
				*ppszRE = CharTraits::Next(*ppszRE);
				return p;
			}
			szAbbrevs++;
		}
		return -1;
	}

	int ParseSE(const RECHAR **ppszRE)
	{

		if (MatchToken(ppszRE, '{'))
			return ParseArg(ppszRE);
		if (MatchToken(ppszRE, '('))
			return ParseGroup(ppszRE);
		if (MatchToken(ppszRE, '['))
			return ParseCharClass(ppszRE);

		if (MatchToken(ppszRE, '\\'))
		{
			if (!CharTraits::Isdigit(**ppszRE))
			{
				// check for abbreviations
				int p;
				p = ParseAbbrev(ppszRE);
				if (p >= 0)
					return p;

				// escaped char
				p = AddInstruction(RE_SYMBOL);
				GetInstruction(p).symbol.nSymbol = (int) **ppszRE;
				*ppszRE = CharTraits::Next(*ppszRE);
				return p;
			}
			// previous match
			int nPrev = AddInstruction(RE_PREVIOUS);
			if (nPrev < 0)
				return nPrev;

			GetInstruction(nPrev).prev.nGroup = (int) CharTraits::Strtol(*ppszRE, (RECHAR **) ppszRE, 10);
			if (GetInstruction(nPrev).prev.nGroup < 0 || GetInstruction(nPrev).prev.nGroup >= m_nNumGroups)
			{
				// invalid group was specified
				return -1;
			}
			return nPrev;
		}
		
		if (MatchToken(ppszRE, '!'))
			return ParseNot(ppszRE);

		if (**ppszRE == '}' || **ppszRE == ']' || **ppszRE == ')')
			return -1;

		if (**ppszRE == '\0')
			return -1;

		int p;
		if (**ppszRE == '.')
			p = AddInstruction(RE_ANY);
		else
		{
			p = AddInstruction(RE_SYMBOL);
			GetInstruction(p).symbol.nSymbol = (int) **ppszRE;
		}
		*ppszRE = CharTraits::Next(*ppszRE);
		return p;
	}

	int ParseE(const RECHAR **ppszRE)
	{
		CParseState ParseState(this);
		const RECHAR *sz = *ppszRE;
		
		int nSE;

		int nFirst = ParseSE(ppszRE);
		if (nFirst < 0)
			return nFirst;

		REInstructionType type = RE_MATCH;

		if (MatchToken(ppszRE, '*'))
			if(MatchToken(ppszRE, '?'))
				type = RE_NG_STAR_BEGIN;
			else
				type = RE_STAR_BEGIN;

			
		else if (MatchToken(ppszRE, '+'))
			if(MatchToken(ppszRE, '?'))
				type = RE_NG_PLUS;
			else
				type = RE_PLUS;
			
		else if (MatchToken(ppszRE, '?'))
			if(MatchToken(ppszRE, '?'))
				type = RE_NG_QUESTION;
			else
				type = RE_QUESTION;


		if (type == RE_MATCH)
			return nFirst;

		if (type == RE_STAR_BEGIN || type == RE_QUESTION|| type == RE_NG_STAR_BEGIN || type == RE_NG_QUESTION)
		{
			ParseState.Restore(this);
		}
		*ppszRE = sz;
		
		int nE;

		if (type == RE_NG_STAR_BEGIN || type == RE_NG_PLUS || type == RE_NG_QUESTION) // Non-Greedy
		{			
			int nCall = AddInstruction(RE_CALL);

			nSE = ParseSE(ppszRE);
			if (nSE < 0)
				return nSE;

			*ppszRE = CharTraits::Next(*ppszRE);
			*ppszRE = CharTraits::Next(*ppszRE);

			if (type == RE_NG_STAR_BEGIN || type == RE_NG_PLUS)
			{
				int nJmp = AddInstruction(RE_JMP);
				GetInstruction(nCall).call.nTarget = nJmp+1;
				GetInstruction(nJmp).jmp.nTarget = nCall;
			}
			else
				GetInstruction(nCall).call.nTarget = nSE+1;

			if (type == RE_NG_PLUS)
				nE = nFirst;
			else
				nE = nCall;
		}
		else // Greedy
		{

			int nPushMem = AddInstruction(RE_PUSH_MEMORY);
			int nStore = AddInstruction(RE_STORE_CHARPOS);
			AddInstruction(RE_PUSH_CHARPOS);
			int nCall = AddInstruction(RE_CALL);
			AddInstruction(RE_POP_CHARPOS);
			int nPopMem = AddInstruction(RE_POP_MEMORY);
			int nJmp = AddInstruction(RE_JMP);

			GetInstruction(nPushMem).memory.nIndex = m_nRequiredMem++;
			GetInstruction(nStore).memory.nIndex = GetInstruction(nPushMem).memory.nIndex;
			GetInstruction(nCall).call.nTarget = nJmp+1;
			GetInstruction(nPopMem).memory.nIndex = GetInstruction(nPushMem).memory.nIndex;

			nSE = ParseSE(ppszRE);
			if (nSE < 0)
				return nSE;

			*ppszRE = CharTraits::Next(*ppszRE);


			int nRetNoMatch = AddInstruction(RE_RET_NOMATCH);
			int nStore1 = AddInstruction(RE_STORE_CHARPOS);

			GetInstruction(nRetNoMatch).memory.nIndex = GetInstruction(nPushMem).memory.nIndex;
			GetInstruction(nStore1).memory.nIndex = GetInstruction(nPushMem).memory.nIndex;

			if (type != RE_QUESTION)
			{
				int nJmp1 = AddInstruction(RE_JMP);
				GetInstruction(nJmp1).jmp.nTarget = nPushMem;
			}

			GetInstruction(nJmp).jmp.nTarget = m_Instructions.GetCount();
			if (type == RE_PLUS)
				nE = nFirst;
			else
				nE = nPushMem;
		}

		return nE;
	}


	int ParseAltE(const RECHAR **ppszRE)
	{
		const RECHAR *sz = *ppszRE;
		CParseState ParseState(this);

		int nPush = AddInstruction(RE_PUSH_CHARPOS);
		int nCall = AddInstruction(RE_CALL);
		GetInstruction(nCall).call.nTarget = nPush+4;
		AddInstruction(RE_POP_CHARPOS);
		int nJmpNext = AddInstruction(RE_JMP);


		int nE = ParseE(ppszRE);
		if (nE < 0)
		{
			ParseState.Restore(this);
			return nE;
		}

		int nJmpEnd = AddInstruction(RE_JMP);
		GetInstruction(nJmpNext).jmp.nTarget = nJmpEnd+1;

		if (!MatchToken(ppszRE, '|'))
		{
			ParseState.Restore(this);
			*ppszRE = sz;

			return ParseE(ppszRE);
		}

		int nAltE = ParseAltE(ppszRE);
		GetInstruction(nJmpEnd).jmp.nTarget = m_Instructions.GetCount();
		GetInstruction(nJmpNext).jmp.nTarget = nAltE;
		if (nAltE < 0)
		{
			ParseState.Restore(this);
			return nAltE;
		}
		return nPush;
	}

	int ParseRE(const RECHAR **ppszRE)
	{
		if (**ppszRE == '\0')
			return -1;

		int p = ParseAltE(ppszRE);
		if (p < 0)
			return p;

		ParseRE(ppszRE);
		return p;
	}

	void FixupMatchContext(CAtlREMatchContext<CharTraits> *pContext, const RECHAR *szOrig, const RECHAR *szNew)
	{
		pContext->m_Match.szStart = szOrig + (pContext->m_Match.szStart - szNew);
		pContext->m_Match.szEnd = szOrig + (pContext->m_Match.szEnd - szNew);
		for (int i=0; i<pContext->m_nNumGroups; i++)
		{
			pContext->m_pMatches[i].szStart = szOrig + (pContext->m_pMatches[i].szStart - szNew);
			pContext->m_pMatches[i].szEnd = szOrig + (pContext->m_pMatches[i].szEnd - szNew);
		}
	}

	int Match(const RECHAR *szIn, CAtlREMatchContext<CharTraits> *pContext, const RECHAR **ppszEnd=NULL)
	{
		if (!szIn || !pContext)
			return 0;

		if (ppszEnd)
			*ppszEnd = NULL;

		const RECHAR *szInput = szIn;
		
		if (!m_bCaseSensitive)
		{
			int nSize = (CharTraits::Strlen(szIn)+1)*sizeof(RECHAR);
			szInput = (const RECHAR *) malloc(nSize);
			if (!szInput)
				return 0;
			
			memcpy((char *) szInput, szIn, nSize);
			CharTraits::Strlwr((RECHAR *) szInput);
		}

		pContext->Initialize(m_nRequiredMem, m_nNumGroups);

		int ip = 0;

		const RECHAR *sz = szInput;
		const RECHAR *szCurrInput = szInput;
		
		while (1)
		{
			if (ip == 0)
				pContext->m_Match.szStart = sz;
			switch (GetInstruction(ip).type)
 			{
			case RE_NOP:
				ip++;
				break;

			case RE_SYMBOL:
				if (GetInstruction(ip).symbol.nSymbol == *sz)
				{
					sz = CharTraits::Next(sz);
					ip++;
				}
				else
				{
					ip = (int) pContext->Pop();
					if (ip < 0)
						goto Error;
				}
				break;

			case RE_ANY:
				if (*sz)
				{
					sz = CharTraits::Next(sz);
					ip++;
				}
				else
				{
					ip = (int) pContext->Pop();
					if (ip < 0)
						goto Error;
				}
				break;

			case RE_GROUP_START:
				pContext->m_pMatches[GetInstruction(ip).group.nGroup].szStart = sz;
				ip++;
				break;

			case RE_GROUP_END:
				pContext->m_pMatches[GetInstruction(ip).group.nGroup].szEnd = sz;
				ip++;
				break;

			case RE_GROUP_RESET:
				// review: they have this as a -1
				pContext->m_pMatches[GetInstruction(ip).group.nGroup].szStart = NULL;;
				pContext->m_pMatches[GetInstruction(ip).group.nGroup].szEnd = NULL;;
				ip++;
				break;

			case RE_PUSH_CHARPOS:
				pContext->Push((void *) sz);
				ip++;
				break;

			case RE_POP_CHARPOS:
				sz = (RECHAR *) pContext->Pop();
				ip++;
				break;

			case RE_CALL:
				pContext->Push(ip+1);
				ip = GetInstruction(ip).call.nTarget;
				break;

			case RE_JMP:
				ip = GetInstruction(ip).jmp.nTarget;
				break;

			case RE_RETURN:
				ip = (int) pContext->Pop();
				if (ip < 0)
					goto Error;
				break;

			case RE_PUSH_MEMORY:
				pContext->Push((void *) (pContext->m_pMem[GetInstruction(ip).memory.nIndex]));
				ip++;
				break;

			case RE_POP_MEMORY:
				pContext->m_pMem[GetInstruction(ip).memory.nIndex] = pContext->Pop();
				ip++;
				break;

			case RE_STORE_CHARPOS:
				pContext->m_pMem[GetInstruction(ip).memory.nIndex] = (void *) sz;
				ip++;
				break;

			case RE_GET_CHARPOS:
				sz = (RECHAR *) pContext->m_pMem[GetInstruction(ip).memory.nIndex];
				ip++;
				break;

			case RE_STORE_STACKPOS:
				pContext->m_pMem[GetInstruction(ip).memory.nIndex] = (void *) pContext->m_nTos;
				ip++;
				break;

			case RE_GET_STACKPOS:
				pContext->m_nTos = (int) pContext->m_pMem[GetInstruction(ip).memory.nIndex];
				ip++;
				break;

			case RE_RET_NOMATCH:
				if (sz == (RECHAR *) pContext->m_pMem[GetInstruction(ip).memory.nIndex])
				{
					// do a return
					ip = (int) pContext->Pop();
					if (ip < 0)
						goto Error;
				}
				else
					ip++;
				break;

			case RE_FAIL:
				sz = CharTraits::Next(szCurrInput);
				szCurrInput = sz;
				if (*sz == '\0')
					goto Error;
				ip = 0;
				pContext->m_nTos = 0;
				break;
		
			case RE_RANGE:
				{
					if (*sz == '\0')
					{
						ip = (int) pContext->Pop();
						if (ip < 0)
							goto Error;
						break;
					}

					unsigned char *pBits = (unsigned char *) (&m_Instructions[ip]+1);
					unsigned int u = (unsigned int) *sz;
					if (pBits[u >> 3] & 1 << (u & 0x7))
					{
						ip += (256/8) / sizeof(INSTRUCTION) + ((256/8) % sizeof(INSTRUCTION) ? 1 : 0);
						ip++;
						sz = CharTraits::Next(sz);
					}
					else
					{
						ip = (int) pContext->Pop();
						if (ip < 0)
							goto Error;
					}
				}
				break;

			case RE_NOTRANGE:
				{
					if (*sz == '\0')
					{
						ip = (int) pContext->Pop();
						if (ip < 0)
							goto Error;
						break;
					}

					unsigned char *pBits = (unsigned char *) (&m_Instructions[ip]+1);
					unsigned int u = (unsigned int) * ((unsigned char *) sz);
					if (pBits[u >> 3] & 1 << (u & 0x7))
					{
						ip = (int) pContext->Pop();
						if (!ip)
							goto Error;
					}
					else
					{
						ip += (256/8) / sizeof(INSTRUCTION) + ((256/8) % sizeof(INSTRUCTION) ? 1 : 0);
						ip++;
						sz = CharTraits::Next(sz);
					}
				}
				break;

			case RE_RANGE_EX:
				{
					if (*sz == '\0')
					{
						ip = (int) pContext->Pop();
						if (ip < 0)
							goto Error;
						break;
					}

					BOOL bMatch = FALSE;
					int inEnd = GetInstruction(ip).range.nTarget;
					ip++;

					while (ip < inEnd)
					{
						if (*sz >= GetInstruction(ip).memory.nIndex && *sz <= GetInstruction(ip+1).memory.nIndex)
						{
							// if we match, we jump to the end
							sz = CharTraits::Next(sz);
							ip = inEnd;
							bMatch = TRUE;
						}
						else
						{
							ip += 2;
						}
					}
					if (!bMatch)
					{
						ip = (int) pContext->Pop();
						if (ip < 0)
							goto Error;
					}
				}
				break;

			case RE_NOTRANGE_EX:
				{
					if (*sz == '\0')
					{
						ip = (int) pContext->Pop();
						if (ip < 0)
							goto Error;
						break;
					}

					BOOL bMatch = TRUE;
					int inEnd = GetInstruction(ip).range.nTarget;
					ip++;

					while (ip < inEnd)
					{
						if (*sz >= GetInstruction(ip).memory.nIndex && *sz <= GetInstruction(ip+1).memory.nIndex)
						{
							ip = (int) pContext->Pop();
							if (ip < 0)
								goto Error;
							bMatch = FALSE;
							break;
						}
						else
						{
							// if we match, we jump to the end
							ip += 2;
						}
					}
					if (bMatch)
						sz = CharTraits::Next(sz);
				}
				break;

			case RE_PREVIOUS:
				{
					BOOL bMatch = FALSE;
					if (m_bCaseSensitive)
						bMatch = !CharTraits::Strncmp(sz, pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szStart,
							pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szEnd-pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szStart);
					else
						bMatch = !CharTraits::Strnicmp(sz, pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szStart,
							pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szEnd-pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szStart);
					if (bMatch)
					{
						sz += pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szEnd-pContext->m_pMatches[GetInstruction(ip).prev.nGroup].szStart;
						ip++;
						break;
					}
					ip = (int) pContext->Pop();
					if (ip < 0)
						goto Error;
				}
				break;

			case RE_MATCH:
				pContext->m_Match.szEnd = sz;
				if (!m_bCaseSensitive)
					FixupMatchContext(pContext, szIn, szInput);
				if (ppszEnd)
					*ppszEnd = szIn + (sz - szInput);
				if (szInput != szIn)
					free((void *) szInput);
				return 1;
				break;

			default:
				ATLASSERT(FALSE);
				break;
			}
		}

		ATLASSERT(FALSE);
Error:
		pContext->m_Match.szEnd = sz;
		if (!m_bCaseSensitive)
			FixupMatchContext(pContext, szIn, szInput);
		if (ppszEnd)
			*ppszEnd = szIn + (sz - szInput);
		if (szInput != szIn)
			free((void *) szInput);
		return 0;
	}

	// CAtlRegExp::Reset
	// Removes all instructions to allow reparsing into the same instance
	void Reset()
	{
		m_Instructions.RemoveAll();
		m_nRequiredMem = 0;
		m_bCaseSensitive = TRUE;
		m_nNumGroups = 0;
	}
	
	// CAtlRegExp::Parse
	// Parses the regular expression
	// returns TRUE if successful, FALSE otherwise
	BOOL Parse(const RECHAR *szRE, BOOL bCaseSensitive=TRUE)
	{
		Reset();
		
		m_bCaseSensitive = bCaseSensitive;

		const RECHAR *szInput = szRE;

		if (!bCaseSensitive)
		{
			// copy the string
			int nSize = (CharTraits::Strlen(szRE)+1)*sizeof(RECHAR);
			szInput = (const RECHAR *) malloc(nSize);
			if (!szInput)
				return FALSE;
			
			memcpy((char *) szInput, szRE, nSize);

			CharTraits::Strlwr((char *) szInput);
		}
		const RECHAR *sz = szInput;

		int nCall = AddInstruction(RE_CALL);
		AddInstruction(RE_FAIL);

		int nRE = ParseRE(&sz);
		if (nRE > 0)
		{
			GetInstruction(nCall).call.nTarget = 2;

			AddInstruction(RE_MATCH);
		}
		
		if (szInput != szRE)
			free((void *) szInput);

		return (nRE > 0) ? TRUE : FALSE;
	}

#ifdef ATL_REGEXP_DUMP
	int Dump()
	{
		int ip = 0;

		while (1)
		{
			printf("%08x ", ip);
			switch (GetInstruction(ip).type)
			{
			case RE_NOP:
				printf("NOP\n");
				ip++;
				break;

			case RE_SYMBOL:
				printf("Symbol %c\n", GetInstruction(ip).symbol.nSymbol);
				ip++;
				break;

			case RE_ANY:
				printf("Any\n");
				ip++;
				break;

			case RE_RANGE:
				printf("Range\n");
				ip++;
				ip += (256/8) / sizeof(INSTRUCTION) + ((256/8) % sizeof(INSTRUCTION) ? 1 : 0);
				break;

			case RE_NOTRANGE:
				printf("NOT Range\n");
				ip++;
				ip += (256/8) / sizeof(INSTRUCTION) + ((256/8) % sizeof(INSTRUCTION) ? 1 : 0);
				break;

			case RE_RANGE_EX:
				printf("RangeEx %08x\n", GetInstruction(ip).range.nTarget);
				ip++;
				break;

			case RE_NOTRANGE_EX:
				printf("NotRangeEx %08x\n", GetInstruction(ip).range.nTarget);
				ip++;
				break;

			case RE_GROUP_START:
				printf("Start group %d\n", GetInstruction(ip).group.nGroup);
				ip++;
				break;

			case RE_GROUP_END:
				printf("Group end %d\n", GetInstruction(ip).group.nGroup);
				ip++;
				break;

			case RE_GROUP_RESET:
				printf("Group reset %d\n", GetInstruction(ip).group.nGroup);
				ip++;
				break;

			case RE_PUSH_CHARPOS:
				printf("Push char pos\n");
				ip++;
				break;

			case RE_POP_CHARPOS:
				printf("Pop char pos\n");
				ip++;
				break;

			case RE_STORE_CHARPOS:
				printf("Store char pos %d\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_GET_CHARPOS:
				printf("Get char pos %d\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_STORE_STACKPOS:
				printf("Store stack pos %d\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_GET_STACKPOS:
				printf("Get stack pos %d\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_CALL:
				printf("Call %08x\n", GetInstruction(ip).call.nTarget);
				ip++;
				break;

			case RE_JMP:
				printf("Jump %08x\n", GetInstruction(ip).jmp.nTarget);
				ip++;
				break;

			case RE_RETURN:
				printf("return\n");
				ip++;
				break;

			case RE_PUSH_MEMORY:
				printf("Push memory %08x\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_POP_MEMORY:
				printf("Pop memory %08x\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_RET_NOMATCH:
				printf("Return no match %08x\n", GetInstruction(ip).memory.nIndex);
				ip++;
				break;

			case RE_MATCH:
				printf("END\n");
				return 1;
				break;

			case RE_FAIL:
				printf("FAIL\n");
				ip++;
				break;

			case RE_PREVIOUS:
				printf("Prev %d\n", GetInstruction(ip).prev.nGroup);
				ip++;
				break;

			default:
				printf("????\n");
				ip++;
				break;
			}
		}
	}
#endif
};

} // namespace ATL
