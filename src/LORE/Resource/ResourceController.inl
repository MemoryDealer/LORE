#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

template<typename T>
inline void ResourceGroup::insertResource( T* resource, const bool autoDuplicate )
{
  ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
  registry.insert( resource->getName(), resource, autoDuplicate );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline bool ResourceGroup::resourceExists( const string& id )
{
  ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
  return registry.exists( id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline T* ResourceGroup::getResource( const string& id )
{
  ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
  return static_cast< T* >( registry.get( id ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline void ResourceGroup::removeResource( const string& id )
{
  ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
  registry.remove( id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline typename std::enable_if<std::is_base_of<Alloc<ResourceType>, ResourceType>::value, ResourceType*>::type
ResourceController::create( const string& name, const string& groupName, const bool autoDuplicate )
{
  static_assert( std::is_base_of<IResource, ResourceType>::value, "ResourceType must derived from IResource" );
  ResourceType* resource = nullptr;

  // Allocate object directly in LORE library.
  resource = MemoryAccess::GetPrimaryPoolCluster()->create<ResourceType>();
  resource->setName( name );
  resource->setResourceGroupName( groupName );
  _getGroup( groupName )->insertResource<ResourceType>( resource, autoDuplicate );
  return resource;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline typename std::enable_if<!std::is_base_of<Alloc<ResourceType>, ResourceType>::value, ResourceType*>::type
ResourceController::create( const string& name, const string& groupName, const bool autoDuplicate )
{
  static_assert( std::is_base_of<IResource, ResourceType>::value, "ResourceType must derived from IResource" );
  ResourceType* resource = nullptr;

  // This is a derived resource that is defined in the render plugin. Call the functor bound
  // for this type to get the allocated result.
  auto functor = getCreationFunctor<ResourceType>();
  if ( functor ) {
    resource = static_cast< ResourceType* >( functor() );
    resource->setName( name );
    resource->setResourceGroupName( groupName );
    _getGroup( groupName )->insertResource<ResourceType>( resource, autoDuplicate );
  }
  return resource;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline bool ResourceController::resourceExists( const string& name, const string& groupName )
{
  return ( _getGroup( groupName )->resourceExists<ResourceType>( name ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline ResourceType* ResourceController::get( const string& name, const string& groupName )
{
  return _getGroup( groupName )->getResource<ResourceType>( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline typename std::enable_if<std::is_base_of<Alloc<ResourceType>, ResourceType>::value, void>::type
ResourceController::destroy( ResourceType* resource )
{
  if ( !MemoryAccess::GetPrimaryPoolCluster()->poolExists<ResourceType>() ) {
    //LogWrite( warning, "Tried to delete resource from non-existent pool" );
    return;
  }

  auto groupName = resource->getResourceGroupName();
  _getGroup( groupName )->removeResource<ResourceType>( resource->getName() );
  MemoryAccess::GetPrimaryPoolCluster()->destroy<ResourceType>( resource );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline typename std::enable_if<!std::is_base_of<Alloc<ResourceType>, ResourceType>::value, void>::type
ResourceController::destroy( ResourceType* resource )
{
  if ( !MemoryAccess::GetPrimaryPoolCluster()->poolExists<ResourceType>() ) {
    //LogWrite( warning, "Tried to delete resource from non-existent pool" );
    return;
  }

  auto groupName = resource->getResourceGroupName();
  _getGroup( groupName )->removeResource<ResourceType>( resource->getName() );
  auto functor = getDestructionFunctor<ResourceType>();
  if ( functor ) {
    functor( resource );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename ResourceType>
inline void ResourceController::destroyAllInGroup( const string& groupName )
{
  const std::type_info& ti = typeid( ResourceType );
  LogWrite( Info, "Destroying all resources of type %s in group %s", ti.name(), groupName.c_str() );
  const auto t = std::type_index( typeid( ResourceType ) );
  auto group = _getGroup( groupName );
  auto registry = group->_resources[t];
  auto it = registry.getConstIterator();
  while ( it.hasMore() ) {
    destroy<ResourceType>( static_cast< ResourceType* >( it.getNext() ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline void ResourceController::addCreationFunctor( PluginCreationFunctor functor )
{
  const auto t = std::type_index( typeid( T ) );
  _creationFunctors[t] = functor;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline void ResourceController::addDestructionFunctor( PluginDestructionFunctor functor )
{
  const auto t = std::type_index( typeid( T ) );
  _destructionFunctors[t] = functor;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline PluginCreationFunctor ResourceController::getCreationFunctor()
{
  const auto t = std::type_index( typeid( T ) );
  auto lookup = _creationFunctors.find( t );
  if ( _creationFunctors.end() != lookup ) {
    return lookup->second;
  }
  return nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<typename T>
inline PluginDestructionFunctor ResourceController::getDestructionFunctor()
{
  const auto t = std::type_index( typeid( T ) );
  auto lookup = _destructionFunctors.find( t );
  if ( _destructionFunctors.end() != lookup ) {
    return lookup->second;
  }
  return nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
