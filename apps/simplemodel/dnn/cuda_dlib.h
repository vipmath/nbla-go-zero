// Copyright (C) 2015  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_DNN_CuDA_H_
#define DLIB_DNN_CuDA_H_


#include "tensor.h"

namespace dlib
{
    namespace cuda 
    {

    // ----------------------------------------------------------------------------------------

        void set_device (
            int dev
        );

        int get_device (
        );

        int get_num_devices (
        );

        std::string get_device_name (
            int device
        );

        void set_current_device_blocking_sync(
        );

        bool can_access_peer (int device_id, int peer_device_id);
        bool can_access_peer (const tensor& device, const tensor& peer_device);

        void device_synchronize (int dev);
        void device_synchronize (const tensor& dev);


        class raii_set_device
        {
        public:
            raii_set_device() = delete;
            raii_set_device(const raii_set_device&) = delete;
            raii_set_device& operator=(const raii_set_device&) = delete;

            raii_set_device(int dev)
            {
                prev_dev = get_device();
                set_device(dev);
            }

            raii_set_device(const tensor& dev)
            {
                prev_dev = get_device();
                set_device(dev.device_id());
            }

            void operator() (int dev)
            {
                set_device(dev);
            }

            void operator() (const tensor& dev)
            {
                set_device(dev.device_id());
            }

            ~raii_set_device() noexcept(false)
            {
                set_device(prev_dev);
            }

        private:
            int prev_dev;
        };


#ifdef DLIB_USE_CUDA

        class enable_peer_access
        {
        public:

            enable_peer_access() = delete;
            enable_peer_access(const enable_peer_access&) = delete;
            enable_peer_access& operator=(const enable_peer_access&) = delete;

            enable_peer_access(
                int device_id,
                int peer_device_id
            );

            enable_peer_access(
                const tensor& device,
                const tensor& peer_device
            ) : enable_peer_access(device.device_id(), peer_device.device_id())
            {}

            ~enable_peer_access() noexcept(false);

        private:

            bool call_disable;
            int device_id;
            int peer_device_id;
        };

    // ------------------------------------------------------------------------------------

        void set_tensor (
            tensor& t,
            float value
        );

        void scale_tensor (
            tensor& t,
            float value
        );

        void add (
            tensor& dest,
            const tensor& src1,
            const tensor& src2
        );

        // Note that this function isn't in the tt:: namespace because add_scaled() is
        // called by cuda::add() so we don't need a tt:: version of add_scaled().  
        void add_scaled(
            tensor& dest,
            const float scale,
            const tensor& src
        );

        void add_cv_to_all_columns(
            float beta, 
            tensor& dest, 
            float alpha, 
            const tensor& src
        );

    // -----------------------------------------------------------------------------------

        void affine_transform_conv(
            tensor& dest,
            const tensor& src,
            const tensor& A,
            const tensor& B
        );
        
#else // if DLIB_USE_CUDA NOT DEFINED

        inline void set_device (
            int id
        )
        {
            DLIB_CASSERT(id == 0, "dlib::cuda::set_device(id) called with an invalid device id.");
        }

        inline int get_device (
        ){ return 0; }

        inline int get_num_devices (
        ) { return 1; }

        inline std::string get_device_name (
            int device
        ) 
        {
            DLIB_CASSERT(device == 0, "dlib::cuda::set_device(id) called with an invalid device id.");
            return "CUDA_DISABLED";
        }

        inline void set_current_device_blocking_sync(
        ) {}


        inline bool can_access_peer (int , int )
        { return false; }
        inline bool can_access_peer (const tensor& , const tensor& )
        { return false; }

        inline void device_synchronize (int ){}
        inline void device_synchronize (const tensor& ){}

        class enable_peer_access
        {
        public:
            enable_peer_access() = delete;
            enable_peer_access(const enable_peer_access&) = delete;
            enable_peer_access& operator=(const enable_peer_access&) = delete;
            enable_peer_access( int, int ){}
            enable_peer_access( const tensor&, const tensor& ) {}
        };

#endif // DLIB_USE_CUDA

    } 
}


#endif // DLIB_DNN_CuDA_H_
