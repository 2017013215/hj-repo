#ifndef BYY_SILVERLINING_EXPORT_H
#define BYY_SILVERLINING_EXPORT_H 1

#ifdef SNIGSILVERLINING_LIB
# define SNIGSILVERLINING_EXPORT __declspec(dllexport)
#else
# define SNIGSILVERLINING_EXPORT __declspec(dllimport)
#endif

#endif // BYY_SILVERLINING_EXPORT_H

