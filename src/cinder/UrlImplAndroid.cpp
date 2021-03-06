/*
 Copyright (c) 2011, The Cinder Project (http://libcinder.org)
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/UrlImplAndroid.h"

namespace cinder {

IStreamUrlImplAndroid::IStreamUrlImplAndroid( const std::string &url, const std::string &user, const std::string &password )
	: IStreamUrlImpl( user, password )
{	
}

IStreamUrlImplAndroid::~IStreamUrlImplAndroid()
{
}

bool IStreamUrlImplAndroid::isEof() const
{
    return true;
}

void IStreamUrlImplAndroid::seekRelative( off_t relativeOffset )
{
}

void IStreamUrlImplAndroid::seekAbsolute( off_t absoluteOffset )
{
}

off_t IStreamUrlImplAndroid::tell() const
{
    return 0;
}

off_t IStreamUrlImplAndroid::size() const
{
    return 0;
}

void IStreamUrlImplAndroid::IORead( void *dest, size_t size )
{
}

size_t IStreamUrlImplAndroid::readDataAvailable( void *dest, size_t maxSize )
{
    return 0;
}

} // namespace cinder
