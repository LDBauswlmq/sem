#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
//#include "read_xml.hpp"

class Stage {
public:
	int maxWeakCount;
	long double threshold;
};

class WeakClassifier {
public:
	int rectNumber;
	long double nodeValue;
	long double leftValue;
	long double rightValue;
};

class ClassifierRect {
public:
	int *x = new int[3];
	int *y = new int[3];
	int *width = new int[3];
	int *height = new int[3];
        int featuretype1;
        int featuretype2;

};

static int mWC_i;
static int sTs_i;
static int iN_i;//internalNodes
static int iNlf_i;//internalNodes leafValues
static int rects_i;
char *pEnd1;
char *pEnd2;
char *pEnd3;
char *pEnd4;
char *pEnd5;

static void
get_element_info(xmlNode *a_node, Stage *stages, WeakClassifier *wk, ClassifierRect *rects){
    xmlNode *cur_node = NULL;
    int trash;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)cur_node->name,"maxWeakCount")==0){
                stages[mWC_i].maxWeakCount = atoi((char *)cur_node->children->content);
                printf("stages[%d] value: %d\n",mWC_i,stages[mWC_i].maxWeakCount);
                mWC_i++;
            }else if(strcmp((char *)cur_node->name,"stageThreshold")==0){
                stages[sTs_i].threshold = strtold((char *)cur_node->children->content, NULL);
                printf("stages[%d] threshold: %.19Lf\n",sTs_i,stages[sTs_i].threshold);
                sTs_i++;
            }else if(strcmp((char *)cur_node->name,"internalNodes")==0){
                trash = (int)strtold((char *)cur_node->children->content, &pEnd1);
                trash = (int)strtold(pEnd1, &pEnd2);
                wk[iN_i].rectNumber = (int)strtold(pEnd2, &pEnd3);
                wk[iN_i].nodeValue = strtold(pEnd3, &pEnd4);
                printf("wk[%d] rectNumber: %d\n",iN_i,wk[iN_i].rectNumber);
                printf("wk[%d] nodeValue: %.19Lf\n",iN_i,wk[iN_i].nodeValue);
                iN_i++;
            }else if(strcmp((char *)cur_node->name,"leafValues")==0){
                wk[iNlf_i].leftValue = strtold((char *)cur_node->children->content, &pEnd1);
                wk[iNlf_i].rightValue = strtold(pEnd1, NULL);
                printf("wk[%d] leftValue: %.19Lf\n",iN_i,wk[iNlf_i].leftValue);
                printf("wk[%d] rightValue: %.19Lf\n",iN_i,wk[iNlf_i].rightValue);
                iNlf_i++;
            }else if(strcmp((char *)cur_node->name,"rects")==0){
                if(cur_node->children->next != 0){
                    rects[rects_i].x[0] = (int)strtold((char *)cur_node->children->next->children->content, &pEnd1);
                    rects[rects_i].y[0] = (int)strtold(pEnd1, &pEnd2);
                    rects[rects_i].width[0] = (int)strtold(pEnd2, &pEnd3);
                    rects[rects_i].height[0] = (int)strtold(pEnd3, &pEnd4);
                    trash = (int)strtold(pEnd4, &pEnd5);
                    rects[rects_i].x[1] = (int)strtold((char *)cur_node->children->next->next->next->children->content, &pEnd1);
                    rects[rects_i].y[1] = (int)strtold(pEnd1, &pEnd2);
                    rects[rects_i].width[1] = (int)strtold(pEnd2, &pEnd3);
                    rects[rects_i].height[1] = (int)strtold(pEnd3, &pEnd4);
                    rects[rects_i].featuretype1 = (int)strtold(pEnd4, &pEnd5);
                
                    printf("rects[%d] sub1 : %d, %d, %d, %d\n",rects_i,rects[rects_i].x[0],
                        rects[rects_i].y[0], rects[rects_i].width[0], rects[rects_i].height[0]);
                    printf("rects[%d] sub2 : %d, %d, %d, %d\n",rects_i,rects[rects_i].x[1],
                        rects[rects_i].y[1], rects[rects_i].width[1], rects[rects_i].height[1]);
                    printf("rects[%d] featuretype1 : %d\n", rects_i, rects[rects_i].featuretype1);
                
                    if(cur_node->children->next->next->next->next != 0){
                        rects[rects_i].x[2] = (int)strtold((char *)cur_node->children->next->next->next->next->next->children->content, &pEnd1);
                        rects[rects_i].y[2] = (int)strtold(pEnd1, &pEnd2);
                        rects[rects_i].width[2] = (int)strtold(pEnd2, &pEnd3);
                        rects[rects_i].height[2] = (int)strtold(pEnd3, &pEnd4);
                        rects[rects_i].featuretype2 = (int)strtold(pEnd4, NULL);
                    
                        printf("rects[%d] sub3 : %d, %d, %d, %d\n",rects_i,rects[rects_i].x[2],
                        rects[rects_i].y[2], rects[rects_i].width[2], rects[rects_i].height[2]);
                        printf("rects[%d] featuretype2 : %d\n", rects_i, rects[rects_i].featuretype2);
                    }
                    rects_i++;
                }
            }
            
        }
        get_element_info(cur_node->children, stages, wk, rects);
    }
}

int
read_xml(Stage *stages, WeakClassifier *wk, ClassifierRect *rects){
    
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile("haarcascade_frontalface_default.xml", NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file\n");
    }

    root_element = xmlDocGetRootElement(doc);

    get_element_info(root_element, stages, wk, rects);

    xmlFreeDoc(doc);

    xmlCleanupParser();

    return 0;
}


int main(void){    
    Stage *stages = new Stage[25];
    WeakClassifier *wk = new WeakClassifier[2913];
    ClassifierRect *rects = new ClassifierRect[2913];
    
    mWC_i = 0;
    sTs_i = 0;
    iN_i = 0;
    iNlf_i = 0;
    
    read_xml(stages, wk, rects);
    
    return 0;
}

