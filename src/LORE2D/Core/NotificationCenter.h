#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files ( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include <LORE2D/Core/Notification.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    class LORE_EXPORT NotificationCenter final
    {

    public:

        // Notification handler function pointer.
        using OnNotify = void (*)( const Notification& n );

        using HandlerList = std::vector<OnNotify>;
        using SubscriptionMap = std::unordered_map<const std::type_info*, HandlerList>;

        struct QueuedNotification{
            Notification n;
            std::type_info* t;
        };
        using NotificationQueue = std::queue<QueuedNotification>;

    public:

        constexpr explicit NotificationCenter();

        ~NotificationCenter();

        // Instance.
        static NotificationCenter& get()
        {
            NotificationCenter& nc = *_instance.get();
            return nc;
        }

        template<typename T>
        void subscribe( OnNotify handler )
        {
            _subscriptions[&typeid( T )].push_back( handler );
        }

        template<typename T>
        void notify( const Notification& n )
        {
            QueuedNotification qn;
            qn.n = n;
            qn.t = const_cast< std::type_info* >( &typeid( T ) );

            _queue.push( qn );
        }

        void fireAllNotifications()
        {
            while ( !_queue.empty() ) {
                QueuedNotification qn = _queue.front();

                // Call all subscribed handler functions.
                for ( const auto& handler : _subscriptions[qn.t] ) {
                    handler( qn.n );
                }

                _queue.pop();
            }
        }

        //
        // Static helper functions.

        static void Initialize()
        {
            _instance = std::make_unique<NotificationCenter>();
        }

        static void Destroy()
        {
            _instance.reset();
        }

        template<typename T>
        static void Subscribe( OnNotify handler )
        {
            NotificationCenter::get().subscribe<T>( handler );
        }

        // TODO: Can the template parameter be removed and get the type_info from Notification?
        template<typename T>
        static void Notify( const Notification& n )
        {
            NotificationCenter::get().notify<T>( n );
        }

        static void FireAllNotifications()
        {
            NotificationCenter::get().fireAllNotifications();
        }

    private:

        static std::unique_ptr<NotificationCenter> _instance;

        SubscriptionMap _subscriptions;
        NotificationQueue _queue;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
