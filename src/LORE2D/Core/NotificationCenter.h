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
#include <LORE2D/Core/Singleton.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class NotificationCenter
    /// \brief Single point through which global notifications are handled.
    /// \details Anyone may subscribe to a notification through the NotificationCenter.
    ///     Notifications of any type may also be sent through the NotificationCenter.
    class LORE_EXPORT NotificationCenter final : public Singleton<NotificationCenter>
    {

    public:

        ///
        /// \typedef NotificationHandler
        /// \brief Function pointer to register a notification handler.
        using NotificationHandler = std::function<void ( const Notification& n )>;

    public:

        constexpr explicit NotificationCenter();

        ~NotificationCenter();

        ///
        /// \brief Registers a notification handler function to be called when a
        ///     notification is fired through the NotificationCenter.
        template<typename T>
        void subscribe( NotificationHandler handler )
        {
            auto t = std::type_index( typeid( T ) );
            _subscriptions[t].push_back( handler );
        }

        template<typename T>
        void unsubscribe( NotificationHandler handler )
        {
            auto t = std::type_index( typeid( T ) );
            auto lookup = _subscriptions.find( t );
            if ( lookup != _subscriptions.end() ) {
                _subscriptions.erase( t );
            }
        }

        ///
        /// \brief Immediately notifies all subscribed handlers for the specified
        ///     notification type.
        template<typename T>
        void post( const Notification& n )
        {
            // Call all subscribed handlers for this notification type.
            auto t = std::type_index( typeid( T ) );
            auto lookup = _subscriptions.find( t );
            if ( _subscriptions.end() != lookup ) {
                for ( const auto& handler : lookup->second ) {
                    handler( n );
                }
            }
        }

        //
        // Static helper functions.

        ///
        /// \copydoc NotificationCenter::subscribe()
        template<typename T>
        static void Subscribe( NotificationHandler handler )
        {
            NotificationCenter::Get().subscribe<T>( handler );
        }

        // TODO: Can the template parameter be removed and get the type_info from Notification?
        ///
        /// \copydoc NotificationCenter::notify()
        template<typename T>
        static void Post( const Notification& n )
        {
            NotificationCenter::Get().post<T>( n );
        }

    private:

        using HandlerList = std::vector<NotificationHandler>;
        using SubscriptionMap = std::unordered_map<std::type_index, HandlerList>;

    private:

        static std::unique_ptr<NotificationCenter> _instance;

        SubscriptionMap _subscriptions;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
