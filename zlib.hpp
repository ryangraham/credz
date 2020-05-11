#include <string>
#include <sstream>
#include <zlib.h>

std::string decompress(const std::string &compressed)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    inflateInit2(&zs, -MAX_WBITS);

    zs.next_in = (Bytef *)compressed.data();
    zs.avail_in = compressed.size();

    int ret;
    char buffer[32768];
    std::string output;

    do
    {
        zs.next_out = reinterpret_cast<Bytef *>(buffer);
        zs.avail_out = sizeof(buffer);

        ret = inflate(&zs, 0);

        if (output.size() < zs.total_out)
        {
            output.append(buffer,
                          zs.total_out - output.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return output;
}
