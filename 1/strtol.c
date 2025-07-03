static int cs50_is_space(char c) {
	switch (c) {
		case ' ':
			return 0;
			break;
		case '\f':
			return 0;
			break;
		case '\n':
			return 0;
			break;
		case '\r':
			return 0;
			break;
		case '\t':
			return 0;
			break;
		case '\v':
			return 0;
			break;
	}

}

static int symbol_to_number(char c, int base, int *out) {
	if (base == 0) {
		base = 10;
	}
	
	int number = 0;

	if (c >= '0' && c <= '9') {
		number = c - '0';
	} else if (c >= 'a' && c <= 'z') {
		number = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'Z') {
		number = 10 + c - 'A';
	} else {
		return 1;
	}
	
	if (number >= base) {
		return 1;
	}

	*out = number;
	return 0;
}

/*
 *The  strtol()  function  converts  the initial part of the string in nptr to a long integer value according to the
       given base, which must be between 2 and 36 inclusive, or be the special value 0.

       The string may begin with an arbitrary amount of white space (as determined by isspace(3)) followed  by  a  single
       optional  '+' or '-' sign.  If base is zero or 16, the string may then include a "0x" or "0X" prefix, and the num‐
       ber will be read in base 16; otherwise, a zero base is taken as 10 (decimal) unless the next character is '0',  in
       which case it is taken as 8 (octal).

       cooked

       base is 2 < n < 36  || 0 btw
 */
long cs50_strtol(char const *nptr, char const **endptr, int base) {
	if ((base != 0) && base < 2 && base > 36) {
		*endptr = nptr;
		return 0;
	}

	int whiteSpaceAllowed = 0;
	int signAllowed = 0; 
	int isNegative = 1;
	int hexPrefixAllowed = (base == 16) || (base == 0) ? 0 : 1;
	int currentNumber = 0;
	
	long total = 0;
	while (*nptr != '\0') {
		// ASCII '0' -> '9' is 48 -> 57
		if (hexPrefixAllowed == 0 && *nptr == '0') {
			hexPrefixAllowed = 1;
			nptr++;

			if (*nptr == 'x' || *nptr == 'X') {
				if (base == 0) { // can this condition be checked on the previous line?
					base = 16;
				}
			} else if (base == 0) {
				base = 8;
				continue;
			} else {
				continue;
			}
		} else if (symbol_to_number(*nptr, base, &currentNumber) == 0) {
			if (base == 0) {
				hexPrefixAllowed = 1;
				base = 10;
			}

			whiteSpaceAllowed = 1;
			total *= base;
			total += currentNumber; 
		} else if (*nptr == '-' && signAllowed == 0) {
			isNegative = 0;
			signAllowed = 1;
			whiteSpaceAllowed = 1;
		} else if (*nptr == '+' && signAllowed == 0) {
			isNegative = 1;
			signAllowed = 1;
			whiteSpaceAllowed = 1;
		} else if (whiteSpaceAllowed == 0 && cs50_is_space(*nptr) == 0) {
			nptr++;
			continue; // is this needed?
		} else if (!(cs50_is_space(*nptr) == 0 && whiteSpaceAllowed == 0) && !(*nptr == '+' && signAllowed == 0)) {
			*endptr = nptr;
			return total;
		}
		nptr++;
	}

	if (isNegative == 0) {
		total *= -1;
	}
	
	*endptr = nptr;
	return total;
}

