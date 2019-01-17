#ifndef H_KAO2_STRING
#define H_KAO2_STRING

#include <cstring> // `std::memcpy`


////////////////////////////////////////////////////////////////
// Klasay napisów w KAO2/KAO3
////////////////////////////////////////////////////////////////

template <typename charT>
class eStringBase
{
	private:

		int refCount;
		int length;
		charT* text;

	public:

		/* Konstrukcja klasy */
		eStringBase<charT>(int count);

		/* Usuwanie, modyfikowanie referencji */
		~eStringBase();
		eStringBase<charT>* decRef();
		void incRef();

		/* Odwołania do wartości prywatnych */
		charT* getText() const;
		int getLength() const;
};

template <typename charT>
class eStringPtrBase
{
	private:

		eStringBase<charT>* pString;

		/* Funkcje związane z konstrukcją */
		void create(int count);
		void create(const charT* str);
		void copy(const eStringPtrBase<charT>& str);

		/* Modyfikowanie referencji */
		void decRef();
		void incRef();

	public:

		/* Funkcje związane z konstrukcją i dekonstrukcją */
		eStringPtrBase<charT>(int count = 0);
		eStringPtrBase<charT>(const charT* str);
		eStringPtrBase<charT>(const eStringPtrBase<charT>& str);
		~eStringPtrBase();

		/* Odwołania do wartości prywatnych */
		charT* getText() const;
		int getLength() const;

		/* Różne funkcje operujące na tekstach */
		bool compare(const charT* str, int pos = 0, int count = 0, bool case_sensitive = false) const;
		bool compare(const eStringPtrBase<charT>& str, int pos = 0, int count = 0, bool case_sensitive = false) const;
		bool hasExtension(const charT* str) const;
		eStringPtrBase<charT> getSubstring(int pos, int count = 0) const;
		eStringPtrBase<charT> getFilename() const;

		/* Operatory przypisywania i kopiowania */
		eStringPtrBase<charT>& operator = (const charT* str);
		eStringPtrBase<charT>& operator = (eStringPtrBase<charT>& str);
		eStringPtrBase<charT> operator + (const charT* str);
		eStringPtrBase<charT> operator + (const eStringPtrBase<charT>& str);
		eStringPtrBase<charT>& operator += (const charT* str);
		eStringPtrBase<charT>& operator += (const eStringPtrBase<charT>& str);
};


////////////////////////////////////////////////////////////////
// Definicje używanych typów tekstu
////////////////////////////////////////////////////////////////

typedef eStringPtrBase<char> eString;
typedef eStringPtrBase<wchar_t> eUnicodeString;


////////////////////////////////////////////////////////////////
// Dodatkowe funkcje szblonowe
////////////////////////////////////////////////////////////////

void _convertString(eString& str1, const eUnicodeString& str2);
void _convertString(eUnicodeString& str1, const eString& str2);

template <typename charT>
int _getCharArrayLength(const charT* str);

template <typename charT>
charT _toLowerCase(charT x);

#endif
