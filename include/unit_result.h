#ifndef UNIT_RESULT_H
#define UNIT_RESULT_H

void testResultInit(void);
void testResultCreateNewNode(void);
void testResultIsLastNodeFull(void);
void testResultAppend(void);
void testResultGetTotalTuples(void);
void testResultRestart(void);
void testIntermediateResultInit(void);
void testIntermediateResultCreateNewNode(void);
void testIntermediateResultAddJoinedRel(void);
void testIntermediateResultIsContained(void);
void testIntermediateResultGetNode(void);
void testIntermediateResultUpdateSize(void);

#endif