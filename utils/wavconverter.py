from sys import stdout

from itertools import izip_longest
def grouper(n, iterable, fillvalue=None):
    args = [iter(iterable)] * n
    return izip_longest(fillvalue=fillvalue, *args)

bytes = []

with open('../resources/sounds/blip.wav', 'rb') as f:
  byte = f.read(1)
  while byte != '':
      bytes.append(byte)
      byte = f.read(1)
with open('../src/blip.h', 'w') as f:
  f.write('#ifndef BLIP_H\n')
  f.write('#define BLIP_H\n\n')
  f.write('#include <cstdint>\n')
  f.write('#include <vector>\n\n')
  f.write('static std::vector<std::uint8_t> blip = {\n')
  for x in grouper(13, bytes):
    f.write('\t')
    for b in [hex(ord(y)) for y in x if y is not None]:
      f.write('%s,\t' % b)
    f.write('\n')
  f.write('};\n')
  f.write('\n#endif /* BLIP_H */\n')

