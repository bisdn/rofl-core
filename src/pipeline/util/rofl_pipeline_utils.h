#ifndef ROFL_PIPELINE_UTILS_H
#define ROFL_PIPELINE_UTILS_H 

/* Extern C for C++ linkage */

#ifdef __cplusplus
	# define ROFL_PIPELINE_BEGIN_DECLS extern "C" {
	# define ROFL_PIPELINE_END_DECLS   }
#else
	# define ROFL_PIPELINE_BEGIN_DECLS
	# define ROFL_PIPELINE_END_DECLS
#endif
#endif /* ROFL_PIPELINE_UTILS_H */
