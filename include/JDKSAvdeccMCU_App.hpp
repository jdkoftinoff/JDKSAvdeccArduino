/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_FixedBuffer.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The AppMessage AVDECC Proxy Protocol Message
///
/// See IEEE Std 1722.1-2013 Annex C.4 and Annex C.5
///
struct AppMessage
{
    ///
    /// \brief AppMessage Constructor
    ///
    /// Creates an AppMessage containing a NOP
    ///
    AppMessage();

    ///
    /// \brief AppMessage Copy Constructor
    /// \param other the AppMessage to copy
    ///
    AppMessage( AppMessage const &other );

    ///
    /// \brief operator =
    /// Assignment operator
    /// \param other the AppMessage to copy
    /// \return *this
    ///
    AppMessage const &operator=( AppMessage const &other );

    ///
    /// \brief clear
    ///
    /// Alias for setNOP
    ///
    void clear() { setNOP(); }

    ///
    /// \brief setNOP
    ///
    /// Set the message type to NOP
    /// See IEEE Std 1722.1-2013 Annex C.5.1.1
    ///
    void setNOP();

    ///
    /// \brief setEntityIdRequest
    ///
    /// Set the message type to ENTITY_ID_REQUEST
    /// See IEEE Std 1722.1-2013 Annex C.5.1.2
    ///
    /// \param apc_primary_mac
    /// \param requested_entity_id
    ///
    void setEntityIdRequest( Eui48 const &apc_primary_mac,
                             Eui64 const &requested_entity_id );

    ///
    /// \brief setEntityIdResponse
    ///
    /// Set the message type to ENTITY_ID_RESPONSE
    /// See IEEE Std 1722.1-2013 Annex C.5.1.3
    ///
    /// \param apc_primary_mac
    /// \param requested_entity_id
    ///
    void setEntityIdResponse( Eui48 const &apc_primary_mac,
                              Eui64 const &requested_entity_id );

    ///
    /// \brief setLinkUp
    ///
    /// Set the message type to LINK_UP
    /// See IEEE Std 1722.1-2013 Annex C.5.1.4
    ///
    /// \param network_port_mac
    ///
    void setLinkUp( Eui48 const &network_port_mac );

    ///
    /// \brief setLinkDown
    ///
    /// Set the message type to LINK_DOWN
    /// See IEEE Std 1722.1-2013 Annex C.5.1.5
    ///
    /// \param network_port_mac
    ///
    void setLinkDown( Eui48 const &network_port_mac );

    ///
    /// \brief setAvdeccFromAps
    ///
    /// Set the message type to AVDECC_FROM_APS
    /// See IEEE Std 1722.1-2013 Annex C.5.1.6
    ///
    /// \param frame The AVDECC message from APS
    /// to be encapsulated into APPDU
    ///
    void setAvdeccFromAps( Frame const &frame );

    ///
    /// \brief setAvdeccFromApc
    ///
    /// Set the message type to AVDECC_FROM_APC
    /// See IEEE Std 1722.1-2013 Annex C.5.1.7
    ///
    /// \param frame The AVDECC message from APC
    /// to be encapsulated into APPDU
    ///
    void setAvdeccFromApc( Frame const &frame );

    ///
    /// \brief setVendor
    ///
    /// Set the message type to VENDOR
    /// See IEEE Std 1722.1-2013 Annex C.5.1.8
    ///
    /// \param vendor_message_type Eui48 code
    /// \param payload the vendor specific message data
    ///
    void setVendor( Eui48 const &vendor_message_type,
                    FixedBuffer const &payload );

    ///
    /// \brief m_appdu
    /// The parsed header and additional payload storage
    ///
    jdksavdecc_fullappdu m_appdu;
};

///
/// \brief The AppMessageParser class
///
/// Consumes bytes one at a time and parses
/// AppMessages from the byte stream
///
class AppMessageParser
{
  public:
    ///
    /// \brief max_appdu_message_size The maximum size of an APPDU message
    /// including headers
    ///
    static const int max_appdu_message_size
        = JDKSAVDECC_APPDU_HEADER_LEN + JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH;

    ///
    /// \brief AppMessageParser
    /// Construct an AppMessageParser object
    ///
    AppMessageParser();

    ///
    /// \brief clear
    ///
    /// Clear the current header parsing state and error count
    ///
    void clear()
    {
        m_header_buffer.setLength( 0 );
        m_error_count = 0;
        m_octets_left_in_payload = 0;
    }

    ///
    /// \brief parse parses one octet from a TCP stream
    ///
    /// This method consumes the octet and returns
    /// a pointer to a fully parsed AppMessage or
    /// 0 if no message was parsed yet.
    ///
    /// \param octet
    /// \return
    ///
    AppMessage *parse( uint8_t octet );

    size_t getErrorCount() const { return m_error_count; }

  protected:
    ///
    /// \brief parseHeader
    /// \param octet
    /// \return
    ///
    AppMessage *parseHeader( uint8_t octet );

    ///
    /// \brief validateHeader
    /// \return
    ///
    AppMessage *validateHeader();

    ///
    /// \brief parsePayload
    /// \param octet
    /// \return
    ///
    AppMessage *parsePayload( uint8_t octet );

    size_t m_octets_left_in_payload;
    size_t m_error_count;
    FixedBufferWithSize<JDKSAVDECC_APPDU_HEADER_LEN> m_header_buffer;
    AppMessage m_current_message;
};

///
/// \brief The AppMessageHandler class
///
/// Dispatch received AppMessages to an appropriate
/// handler
///
class AppMessageHandler
{
  public:
    AppMessageHandler() {}
    virtual ~AppMessageHandler() {}

    virtual void onApp( AppMessage const &msg );

  protected:
    virtual void onAppNop( AppMessage const &msg ) = 0;

    virtual void onAppEntityIdRequest( AppMessage const &msg ) = 0;

    virtual void onAppEntityIdResponse( AppMessage const &msg ) = 0;

    virtual void onAppLinkUp( AppMessage const &msg ) = 0;

    virtual void onAppLinkDown( AppMessage const &msg ) = 0;

    virtual void onAppAvdeccFromAps( AppMessage const &msg ) = 0;

    virtual void onAppAvdeccFromApc( AppMessage const &msg ) = 0;

    virtual void onAppVendor( AppMessage const &msg ) = 0;
};
}
