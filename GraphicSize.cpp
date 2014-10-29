#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>


bool getImageSize(const char *fn, int *x,int *y)
{ 
	char buf[24];
	int len = 0;
	std::ifstream s(fn, std::ios::binary | std::ios::ate );
	// make sure file is at least 24 bytes long..
	len = s.tellg();
	if (len < 24) {
		s.close();
		return false;
	}
	s.seekg(0);

	s.read(buf, 24);

  // Strategy:
  // reading GIF dimensions requires the first 10 bytes of the file
  // reading PNG dimensions requires the first 24 bytes of the file
  // reading JPEG dimensions requires scanning through jpeg chunks
  // In all formats, the file is at least 24 bytes big, so we'll read that always

  // For JPEGs, we need to read the first 12 bytes of each chunk.
  // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
    if (buf[0]==(char)0xff 
		&& buf[1]==(char)0xd8 
		&& buf[2]==(char)0xff 
		&& (buf[3]==(char)0xe0 || buf[3]==(char)0xe1)
		)
    {
        long pos=2;
        while (buf[2]==(char)0xFF)
        {
            if (buf[3]==(char)0xC0 || buf[3]==(char)0xC1 || buf[3]==(char)0xC2 || buf[3]==(char)0xC3 || buf[3]==(char)0xC9 || buf[3]==(char)0xCA || buf[3]==(char)0xCB) 
                break;

			int lh = buf[4];
			int ll = buf[5] & 0xFF;
			int sectionSize = (lh << 8) | ll;

			pos += sectionSize + 2;

            if ( pos < 0 || pos+12>len)
				break;

			s.clear();
			s.seekg(pos);
			s.read(buf+2, 12);
        }
    }

	s.close();

  // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
    if (buf[0]==(char)0xFF && buf[1]==(char)0xD8 && buf[2]==(char)0xFF)
    {     
        *y = (buf[7]<<8) + (buf[8] & 0xff); 
        *x = (buf[9]<<8) + (buf[10] & 0xff); 

        return true;
    }

  // GIF: first three bytes say "GIF", next three give version number. Then dimensions
    if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F')
    { 
        *x = (buf[7]<<8) + (buf[6] & 0xff);
        *y = (buf[9]<<8) + (buf[8] & 0xff);
        return true;
    }

  // PNG: the first frame is by definition an IHDR frame, which gives dimensions
    if ( buf[0]==(char)0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==(char)0x0D && buf[5]==(char)0x0A && buf[6]==(char)0x1A && buf[7]==(char)0x0A && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R')
    {
        *x = (buf[16]<<24) + ( (buf[17] & 0xff) <<16) + ( (buf[18] & 0xff) <<8) + ( (buf[19] & 0xff) <<0);
        *y = (buf[20]<<24) + ( (buf[21] & 0xff) <<16) + ( (buf[22] & 0xff) <<8) + ( (buf[23] & 0xff) <<0);

        return true;
    }

    return false;
}