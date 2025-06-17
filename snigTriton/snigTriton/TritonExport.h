#ifndef BYY_TRITON_EXPORT_H
#define BYY_TRITON_EXPORT_H 1

#ifdef SNIGTRITON_LIB
# define SNIGRITON_EXPORT __declspec(dllexport)
#else
# define SNIGRITON_EXPORT __declspec(dllimport)
#endif

#endif // BYY_TRITON_EXPORT_H

