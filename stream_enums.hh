#ifndef __STREAM_ENUMS_HH__
#define __STREAM_ENUMS_HH__


#ifndef Q_NETWORK_IS_BIG_ENDIAN
#define Q_NETWORK_IS_BIG_ENDIAN 0
#endif

#ifndef Q_HOST_IS_BIG_ENDIAN
#define Q_HOST_IS_BIG_ENDIAN 0
#endif


namespace scolex
{


enum stream_mode_t
{
  STREAM_READ = 0x1 << 1,
  STREAM_WRITE = 0x1 << 2,

  STREAM_READWRITE = STREAM_READ | STREAM_WRITE,
};


enum stream_seek_origin_t
{
  STREAM_SEEK_SET,
  STREAM_SEEK_CUR,
  STREAM_SEEK_END,
};


enum stream_endianness_t
{
  STREAM_ENDIAN_LITTLE = 1,
  STREAM_ENDIAN_BIG = 2,

#if Q_HOST_IS_BIG_ENDIAN
  STREAM_ENDIAN_HOST = STREAM_ENDIAN_BIG,
#else
  STREAM_ENDIAN_HOST = STREAM_ENDIAN_LITTLE,
#endif

#if Q_NETWORK_IS_BIG_ENDIAN
  STREAM_ENDIAN_NETWORK = STREAM_ENDIAN_BIG,
#else
  STREAM_ENDIAN_NETWORK = STREAM_ENDIAN_LITTLE,
#endif
};


} // namespace scolex

#endif /* end __STREAM_ENUMS_HH__ include guard */
