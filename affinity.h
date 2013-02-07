/*
COPYRIGHT 2013 INTEL CORPORATION ALL RIGHTS RESERVED. THE
SOURCE CODE, INFORMATION AND MATERIAL ("MATERIAL") CONTAINED
HEREIN IS OWNED BY INTEL CORPORATION OR ITS SUPPLIERS OR
LICENSORS, AND TITLE TO SUCH MATERIAL REMAINS WITH INTEL
CORPORATION OR ITS SUPPLIERS OR LICENSORS. THE MATERIAL
CONTAINS PROPRIETARY INFORMATION OF INTEL OR ITS SUPPLIERS
AND LICENSORS. THE MATERIAL IS PROTECTED BY WORLDWIDE
COPYRIGHT LAWS AND TREATY PROVISIONS. NO PART OF THE
MATERIAL MAY BE USED, COPIED, REPRODUCED, MODIFIED,
PUBLISHED, UPLOADED, POSTED, TRANSMITTED, DISTRIBUTED OR
DISCLOSED IN ANY WAY WITHOUT INTEL'S PRIOR EXPRESS WRITTEN
PERMISSION. NO LICENSE UNDER ANY PATENT, COPYRIGHT OR OTHER
INTELLECTUAL PROPERTY RIGHTS IN THE MATERIAL IS GRANTED TO
OR CONFERRED UPON YOU, EITHER EXPRESSLY, BY IMPLICATION,
INDUCEMENT, ESTOPPEL OR OTHERWISE. ANY LICENSE UNDER SUCH
INTELLECTUAL PROPERTY RIGHTS MUST BE EXPRESS AND APPROVED BY
INTEL IN WRITING. INCLUDE ANY SUPPLIER COPYRIGHT NOTICES AS
SUPPLIER REQUIRES INTEL TO USE. INCLUDE SUPPLIER TRADEMARKS
OR LOGOS AS SUPPLIER REQUIRES INTEL TO USE, PRECEDED BY AN
ASTERISK. AN ASTERISKED FOOTNOTE CAN BE ADDED AS FOLLOWS:
*THIRD PARTY TRADEMARKS ARE THE PROPERTY OF THEIR RESPECTIVE
OWNERS. UNLESS OTHERWISE AGREED BY INTEL IN WRITING, YOU MAY
NOT REMOVE OR ALTER THIS NOTICE OR ANY OTHER NOTICE EMBEDDED
IN MATERIALS BY INTEL OR INTEL.S SUPPLIERS OR LICENSORS IN
ANY WAY.

 **
 * @author      Zacharia Fadika <zfadika@gmail.com>
 */

#ifndef __AFFINITY_H
#define __AFFINITY_H

extern void   set_affinity(int,int,int);

#include <stdio.h>
#ifdef __FreeBSD__
#include <sys/resource.h>
#include <sys/cpuset.h>
#else
#include <sched.h>
#endif

#if defined(__FreeBSD__)
void set_affinity(int UserRequestedThreads, int numProcs,int threadPinInc) {

	int i;
	cpuset_t mask;
	memset(&mask, 0, sizeof(mask));
	int pinCount=0;
	for(i=0;i< UserRequestedThreads;i++)
	{
		if(pinCount <= (numProcs-1))
                {
			CPU_SET(i, &mask);
			if (cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1,sizeof(mask), &mask) < 0) 
			{
				errx(1, "cpuset_setaffinity");
				return;
			}
			else
			{
				printf ("Memcached threads will be pinned starting at CPU %d, incrementing by %d\n",i, threadPinInc);
				pinCount=pinCount+threadPinInc;
			}
		}
		else
			break;

	}
}

#elif defined(__APPLE_CC__)
void set_affinity(int UserRequestedThreads, int numProcs,int threadPinInc) {
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "WARNING: processor affinity not supported\n");
    }
}
#else
void set_affinity(int UserRequestedThreads, int numProcs,int threadPinInc) {

	int i;
        cpu_set_t mask;
        CPU_ZERO(&mask);
	int pinCount=0;
        for(i=0;i< UserRequestedThreads;i++)
        {

		if(pinCount <= (numProcs-1))
		{

                        CPU_SET(pinCount, &mask);

			/* bind to processor number: pinCount*/

                        if (sched_setaffinity(0, sizeof(mask), &mask) <0)
                        {
                                perror("sched_setaffinity");
                        }
			else
			{
				printf ("Memcached threads will be pinned starting at CPU %d, incrementing by %d\n",i, threadPinInc);
				pinCount=pinCount+threadPinInc;
			}
		}
		else
			break;

        }
}
#endif

#endif
