#ifndef SNOCEAN_GLOBAL_H
#define SNOCEAN_GLOBAL_H

#ifdef SNOCEAN_LIB
# define SNOCEAN_EXPORT __declspec(dllexport)
#else
# define SNOCEAN_EXPORT __declspec(dllimport)
#endif

#endif // SNOCEAN_GLOBAL_H
