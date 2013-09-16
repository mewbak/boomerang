#pragma once
#include "prog.h"
#include <vector>

struct WILD_SEGMENT
{
    unsigned offset;
    unsigned size;
};

class BytePattern
{
public:
    BytePattern(unsigned char *data, unsigned data_size);
    ~BytePattern(void);

    void FlagWildBytes(unsigned offset, unsigned size);

    int Match(unsigned char *context, unsigned size);

private:
    unsigned char *m_data;
    unsigned m_size;

    std::vector<WILD_SEGMENT> m_wild_bytes;
};
