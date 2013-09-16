#include "BytePattern.h"
#include <algorithm>

BytePattern::BytePattern(unsigned char *data, unsigned data_size)
    :m_data(data),
     m_size(data_size)
{
}

BytePattern::~BytePattern(void)
{
}

void BytePattern::FlagWildBytes(unsigned offset, unsigned size)
/**
	flags specified bytes as wild bytes.
	wild bytes won't be matched against
	context data
*/
{
    WILD_SEGMENT w = {offset, size};
    m_wild_bytes.push_back(w);
}

// for the sort method, we have to define less-than
bool operator<(const WILD_SEGMENT& a, const WILD_SEGMENT& b)
{
    return a.offset < b.offset;
}


int BytePattern::Match(unsigned char *context, unsigned size)
/**
	matches the pattern against the context,
	returns any successful matches or
	-1 on no match
*/
{
    // if we have nothing to compare
    // or the procedure is less than 8 bytes
    if(!size || size < m_size || m_size < 8)
        return -1;


    sort(m_wild_bytes.begin(), m_wild_bytes.end());
    std::vector<WILD_SEGMENT>::iterator next_wild = m_wild_bytes.begin();

    for(unsigned pos=0; pos<size - m_size; pos ++)
        {
            unsigned cmp_from = 0, cmp_to;
            bool matched = true;
            while(next_wild != m_wild_bytes.end())
                {
                    cmp_to = next_wild->offset;

                    // compare signature
                    if(memcmp(m_data + cmp_from,
                              context + pos + cmp_from,
                              cmp_to - cmp_from))
                        {
                            // pattern does not match
                            matched = false;
                            break;
                        }

                    cmp_from = cmp_to + next_wild->size;
                    next_wild ++;
                }

            // now the last compare
            if(matched)
                {
                    cmp_to = m_size;
                    // compare signature
                    if(!memcmp(m_data + cmp_from,
                               context + pos + cmp_from,
                               cmp_to - cmp_from))
                        {
                            // pattern match
                            return pos;
                        }
                }

        }
    return -1;
}
