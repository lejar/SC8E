import os
import sys
import textwrap

infile = sys.argv[1]
outfile = sys.argv[2]
outdir, outname = os.path.split(outfile)
header = os.path.splitext(outname)[0]

with open(infile, 'rb') as i:
  data = i.read()

output = """\
#ifndef %(header)s_h
#define %(header)s_h

#include <cstdint>
#include <vector>

static std::vector<std::uint8_t> %(header)s = {
\t%(data)s
};
#endif /* %(header)s_H */
""" % {
  "header": header,
  "data": '\n\t'.join(
    textwrap.wrap(
      ', '.join('0x%02x' % ord(y) for y in data),
    79)
  )
}

if not os.path.exists(outdir):
  os.makedirs(outdir)

with open(outfile, 'w') as f:
  f.write(output)
