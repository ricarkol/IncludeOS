// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015-2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP

#include <sstream>

#include "header.hpp"
#include "time.hpp"

namespace http {

///
/// This is the base class of an HTTP message which contain
/// the headers and optional body
///
class Message {
private:
  ///
  /// Internal class type aliases
  ///
  using Message_body   = std::string;
public:
  ///
  /// Default constructor
  ///
  explicit Message() = default;

  ///
  /// Constructor to specify the limit of how many
  /// fields that can be added to the message
  ///
  /// @param limit Capacity of how many fields can
  /// be added to the message
  ///
  explicit Message(const std::size_t limit) noexcept;

  ///
  /// Default copy constructor
  ///
  Message(const Message&) = default;

  ///
  /// Default move constructor
  ///
  explicit Message(Message&&) noexcept = default;

  ///
  /// Default destructor
  ///
  virtual ~Message() noexcept = default;

  ///
  /// Default copy assignment operator
  ///
  Message& operator = (const Message&) = default;

  ///
  /// Default move assignment operator
  ///
  Message& operator = (Message&&) = default;

  ///
  /// Get a modifiable reference to the header object
  ///
  Header& header() noexcept;

  ///
  /// Get a read-only reference to the header object
  ///
  const Header& header() const noexcept;

  ///
  /// Add an entity to the message
  ///
  /// @param message_body The entity to be
  /// sent with the message
  ///
  /// @return The object that invoked this method
  ///
  Message& add_body(const Message_body& message_body);

  ///
  /// Append a chunk to the entity of the message
  ///
  /// @param chunk A chunk to append to the entity
  ///
  /// @return The object that invoked this method
  ///
  Message& add_chunk(const Message_body& chunk);

  ///
  /// Check if this message has an entity
  ///
  /// @return true if entity is present, false
  /// otherwise
  ///
  bool has_body() const noexcept;

  ///
  /// Get a view of the entity in this the message if
  /// present
  ///
  /// @return A view of the entity in this message
  ///
  std::experimental::string_view body() const noexcept;

  ///
  /// Remove the entity from the message
  ///
  /// @return The object that invoked this method
  ///
  Message& clear_body() noexcept;

  ///
  /// Reset the message as if it was now default
  /// constructed
  ///
  /// @return The object that invoked this method
  ///
  virtual Message& reset() noexcept;

  ///
  /// Get a string representation of this
  /// class
  ///
  /// @return A string representation
  ///
  virtual std::string to_string() const;

  ///
  /// Operator to transform this class
  /// into string form
  ///
  operator std::string () const;

  ///
  /// Get a view of a buffer holding intermediate information
  ///
  /// @return A view of a buffer holding intermediate information
  ///
  const std::experimental::string_view private_field() const noexcept;

  ///
  /// Set the content of the buffer holding intermediate information
  ///
  void set_private_field(const char* base, const size_t length) noexcept;

  inline bool headers_complete() const noexcept;

  inline void set_headers_complete(const bool complete) noexcept;
private:
  ///
  /// Class data members
  ///
  Header                         header_fields_;
  Message_body                   message_body_;
  std::experimental::string_view field_;
  bool         headers_complete_;
}; //< class Message

/**--v----------- Helper Functions -----------v--**/

///
/// Add a set of headers to a message
///
Message& operator << (Message& res, const Header_set& headers);

inline void Message::set_headers_complete(const bool complete) noexcept {
  headers_complete_ = complete;
}

inline bool Message::headers_complete() const noexcept {
  return headers_complete_;
}

/**--^----------- Helper Functions -----------^--**/

} //< namespace http

#endif //< HTTP_MESSAGE_HPP
