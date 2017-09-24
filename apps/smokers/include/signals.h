#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(request) \
		ESM_SIGNAL(done)

#define ESM_IDS \
      ESM_ID(tobacco_smoker) \
      ESM_ID(paper_smoker) \
      ESM_ID(matches_smoker) \
      ESM_ID(table)

#define ESM_GROUPS \
      ESM_GROUP(agents) \
      ESM_GROUP(smokers) \

typedef enum
{
	tobacco,
	paper,
	matches
} smoker_resource_e;

#define ESM_SIG_PARAMS \
      smoker_resource_e resource;

#endif /* INCLUDE_SIGNALS_H_ */
