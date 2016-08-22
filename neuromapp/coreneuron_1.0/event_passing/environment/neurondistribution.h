/*
 * neurondistribution.h
 *
 *  Created on: Jul 11, 2016
 *      Author: schumann
 */

#ifndef NEURONDISTRIBUTION_H_
#define NEURONDISTRIBUTION_H_

typedef long unsigned int size_t;

namespace environment
{
    class neurondistribution
    {
    public:
        neurondistribution() {};
        virtual ~neurondistribution() {};
        /**
         *  Get number of local cells
         */
        virtual size_t getlocalcells() const = 0;
        /**
         *  Get number of global cells
         */
        virtual size_t getglobalcells() const = 0;

        /**
         *  Return true if gid is stored locally
         */
        virtual bool isLocal(size_t id) const = 0;
        /**
         *  Maps global ids to local ids
         */
        virtual size_t global2local(size_t glo) const = 0;
        /**
         *  Maps local ids to global ids
         */
        virtual size_t local2global(size_t loc) const = 0;
    };

    class continousdistribution : public neurondistribution {
    public:
        /**
         *  Create continous distribution
         */
        continousdistribution(size_t groups, size_t me, size_t cells);
        /**
         *  Create continous distribution as a subset from given parent distribution
         */
        continousdistribution(size_t groups, size_t me, continousdistribution* parent_distr);

        ~continousdistribution() {};

        inline size_t getlocalcells() const
        {
            return local_number;
        }
        inline size_t getglobalcells() const
        {
            return global_number;
        }
        inline bool isLocal(size_t id) const
        {
            assert(id < global_number);

            return id>=start && id<start+local_number;
        }
        inline size_t global2local(size_t glo) const
        {
            assert(glo>=start);
            assert(glo<start+local_number);

            return  glo - start;
        }
        inline size_t local2global(size_t loc) const
        {
            assert(loc < local_number);

            return start + loc;
        }

    private:
        const size_t global_number;
        size_t local_number;
        size_t start;
    };
};

#endif /* NEURONDISTRIBUTION_H_ */
