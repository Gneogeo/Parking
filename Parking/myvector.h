#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <cstring>
#include <cstdlib>

template <typename T> class myVector {
        myVector(myVector &x); //deactivated copy-constructor
        unsigned int mem;
        unsigned int len;
        T *data;

public:



        myVector() {
                data=0; len=0; mem=0;
        }
        ~myVector() {
                free(data);
        }

        void append(const T &elem) {
                if (mem==len) {
                        if (mem==0) mem=1;
                        else mem*=2;
                        data=(T*)realloc(data,mem*sizeof(T));
                }
		data[len]=elem;
                len++;
        }

        void clear() {
                len=0;
        }

        void truncate() {
                free(data); data=0; len=0; mem=0;
        }
        void truncateInto(unsigned int ln) {
                if (ln<len) len=ln;
        }

        unsigned int length() {return len;}

        const T *getData() {return data;}

        T &at(unsigned int i) {return data[i];}

        void Qsort(int (*compareFunc)(const T *k1,const T *k2)) {
                ::qsort(data,len,sizeof(T),( int(*)(const void *,const void *))compareFunc);
        }


};



#endif /* MYVECTOR_H */
