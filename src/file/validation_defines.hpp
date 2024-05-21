#ifndef FILE_VALIDATION_HPP
#define FILE_VALIDATION_HPP

#include <QObject>
#include <QFile>

const auto default_filename  = QString::fromLatin1("output_%1_%2.dgs");
const auto default_datetime  = QString::fromLatin1("yyyy_MM_dd__hh_mm_ss");
const auto default_signature = QString::fromLatin1("\\x25 \\x44 \\x47 \\x53 \\x%1 \\x%2 \\x%3 \\xDB");

const auto default_body_prefix  = QByteArray{ "\xAB\x57\x46\x41" };
const auto default_body_postfix = QByteArray{ "\x57\x46\x50\xBB" };

const auto version_major = "01";
const auto version_minor = "00";
const auto version_patch = "0A";

#endif // FILE_VALIDATION_HPP
