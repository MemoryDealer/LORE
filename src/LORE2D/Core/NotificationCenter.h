#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016-2017 Jordan Sparks
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
#include <LORE2D/Core/Util.h>

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

        NotificationCenter();

        ~NotificationCenter();

        ///
        /// \brief Registers a notification handler function to be called when a
        ///     notification is fired through the NotificationCenter.
        /// \param T The Notification type to subscribe to.
        /// \param handler Pointer to function that is called when Notification type
        ///     T is triggered.
        template<typename T>
        void subscribe( NotificationHandler handler )
        {
            auto t = std::type_index( typeid( T ) );
            _subscriptions[t].push_back( handler );
        }

        ///
        /// \brief Removes handler from list of registered handlers for specified notification.
        /// \param T The Notification type to unsubscribe from.
        /// \param handler Pointer to function that was registered to handle this Notification.
        template<typename T>
        void unsubscribe( NotificationHandler handler )
        {
            auto t = std::type_index( typeid( T ) );

            // Find list of handlers for this notification type.
            auto lookup = _subscriptions.find( t );
            if ( lookup != _subscriptions.end() ) {
                HandlerList& handlers = lookup->second;

                // Iterate over handlers for this notification type.
                for ( auto it = handlers.begin(); it != handlers.end(); ) {
                    NotificationHandler nh = *it;
                    if ( Util::GetFPAddress( nh ) == Util::GetFPAddress( handler ) ) {
                        it = handlers.erase( it );
                    }
                    else {
                        ++it;
                    }
                }
            }
        }

        ///
        /// \brief Immediately notifies all subscribed handlers for the specified
        ///     notification type.
        template<typename T>
        void post( const Notification& n ) const
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

        ///
        /// \copydoc NotificationHandler::unsubscribe()
        template<typename T>
        static void Unsubscribe( NotificationHandler handler )
        {
            NotificationCenter::Get().unsubscribe<T>( handler );
        }

        // TODO: Can the template parameter be removed and get the type_info from Notification?
        ///
        /// \copydoc NotificationCenter::notify()
        template<typename T>
        static void Post( const Notification& n )
        {
            NotificationCenter::Get().post<T>( n );
        }

        //
        // Helper macros.

#define NotificationSubscribe( Type, Handler ) NotificationCenter::Subscribe<Type>( std::bind( Handler, this, std::placeholders::_1 ) )
#define NotificationUnsubscribe( Type, Handler ) NotificationCenter::Unsubscribe<Type>( std::bind( Handler, this, std::placeholders::_1 ) )

    private:

        using HandlerList = std::vector<NotificationHandler>;
        using SubscriptionMap = std::unordered_map<std::type_index, HandlerList>;

    private:

        SubscriptionMap _subscriptions {};

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
