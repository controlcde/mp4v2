/* mp4tags -- tool to set iTunes-compatible metadata tags
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * Contributed to MPEG4IP
 * by Christopher League <league@contrapunctus.net>
 */

#include "util/impl.h"

using namespace mp4v2::util;

///////////////////////////////////////////////////////////////////////////////

/* One-letter options -- if you want to rearrange these, change them
   here, immediately below in OPT_STRING, and in the help text. */
#define OPT_HELP         'h'
#define OPT_VERSION      'v'
#define OPT_ALBUM        'A'
#define OPT_ARTIST       'a'
#define OPT_TEMPO        'b'
#define OPT_COMMENT      'c'
#define OPT_COPYRIGHT    'C'
#define OPT_DISK         'd'
#define OPT_DISKS        'D'
#define OPT_ENCODEDBY    'e'
#define OPT_TOOL         'E'
#define OPT_GENRE        'g'
#define OPT_GROUPING     'G'
#define OPT_HD           'H'
#define OPT_MEDIA_TYPE   'i'
#define OPT_CNID         'I'
#define OPT_DESCRIPTION  'm'
#define OPT_TVEPISODE    'M'
#define OPT_PICTURE      'P'
#define OPT_SONG         's'
#define OPT_TRACK        't'
#define OPT_TRACKS       'T'
#define OPT_COMPOSER     'w'
#define OPT_YEAR         'y'
#define OPT_REMOVE       'r'
#define OPT_ALBUM_ARTIST 'R'

#define OPT_STRING  "hvA:a:b:c:C:d:D:e:E:g:G:H:i:I:m:M:P:s:t:T:w:y:r:R:"

#define ELEMENT_OF(x,i) x[int(i)]

static const char* const help_text =
    "OPTION... FILE...\n"
    "Adds or modifies iTunes-compatible tags on MP4 files.\n"
    "\n"
    "  -h, -help            Display this help text and exit\n"
    "  -v, -version         Display version information and exit\n"
    "  -A, -album       STR  Set the album title\n"
    "  -a, -artist      STR  Set the artist information\n"
    "  -b, -tempo       NUM  Set the tempo (beats per minute)\n"
    "  -c, -comment     STR  Set a general comment\n"
    "  -C, -copyright   STR  Set the copyright information\n"
    "  -d, -disk        NUM  Set the disk number\n"
    "  -D, -disks       NUM  Set the number of disks\n"
    "  -e, -encodedby   STR  Set the name of the person or company who encoded the file\n"
    "  -E, -tool        STR  Set the software used for encoding\n"
    "  -g, -genre       STR  Set the genre name\n"
    "  -G, -grouping    STR  Set the grouping name\n"
    "  -H, -hdvideo     NUM  Set the HD flag (1\\0)\n"
    "  -i, -type        STR  Set the Media Type(tvshow, movie, music, ...)\n"
    "  -I, -cnid        NUM  Set cnID\n"
    "  -m, -description STR  Set the short description\n"
    "  -M, -episode     NUM  Set the episode number\n"
    "  -P, -picture     PTH  Set the picture as a .png\n"
    "  -s, -song        STR  Set the song title\n"
    "  -t, -track       NUM  Set the track number\n"
    "  -T, -tracks      NUM  Set the number of tracks\n"
    "  -w, -writer      STR  Set the composer information\n"
    "  -y, -year        NUM  Set the release date\n"
    "  -R, -albumartist STR Set the album artist\n"
    "  -r, -remove      STR  Remove tags by code (e.g. \"-r cs\"\n"
    "                        removes the comment and song tags)";

extern "C" int
    main( int argc, char** argv )
{
    const prog::Option long_options[] = {
        { "help",        prog::Option::NO_ARG,       0, OPT_HELP         },
        { "version",     prog::Option::NO_ARG,       0, OPT_VERSION      },
        { "album",       prog::Option::REQUIRED_ARG, 0, OPT_ALBUM        },
        { "artist",      prog::Option::REQUIRED_ARG, 0, OPT_ARTIST       },
        { "comment",     prog::Option::REQUIRED_ARG, 0, OPT_COMMENT      },
        { "copyright",   prog::Option::REQUIRED_ARG, 0, OPT_COPYRIGHT    },
        { "disk",        prog::Option::REQUIRED_ARG, 0, OPT_DISK         },
        { "disks",       prog::Option::REQUIRED_ARG, 0, OPT_DISKS        },
        { "encodedby",   prog::Option::REQUIRED_ARG, 0, OPT_ENCODEDBY    },
        { "tool",        prog::Option::REQUIRED_ARG, 0, OPT_TOOL         },
        { "genre",       prog::Option::REQUIRED_ARG, 0, OPT_GENRE        },
        { "grouping",    prog::Option::REQUIRED_ARG, 0, OPT_GROUPING     },
        { "hdvideo",     prog::Option::REQUIRED_ARG, 0, OPT_HD           },
        { "type",        prog::Option::REQUIRED_ARG, 0, OPT_MEDIA_TYPE   },
        { "cnid",        prog::Option::REQUIRED_ARG, 0, OPT_CNID         },
        { "description", prog::Option::REQUIRED_ARG, 0, OPT_DESCRIPTION  },
        { "episode",     prog::Option::REQUIRED_ARG, 0, OPT_TVEPISODE    },
        { "picture",     prog::Option::REQUIRED_ARG, 0, OPT_PICTURE      },
        { "song",        prog::Option::REQUIRED_ARG, 0, OPT_SONG         },
        { "tempo",       prog::Option::REQUIRED_ARG, 0, OPT_TEMPO        },
        { "track",       prog::Option::REQUIRED_ARG, 0, OPT_TRACK        },
        { "tracks",      prog::Option::REQUIRED_ARG, 0, OPT_TRACKS       },
        { "writer",      prog::Option::REQUIRED_ARG, 0, OPT_COMPOSER     },
        { "year",        prog::Option::REQUIRED_ARG, 0, OPT_YEAR         },
        { "remove",      prog::Option::REQUIRED_ARG, 0, OPT_REMOVE       },
        { "albumartist", prog::Option::REQUIRED_ARG, 0, OPT_ALBUM_ARTIST },
        { NULL, prog::Option::NO_ARG, 0, 0 }
    };

    /* Sparse arrays of tag data: some space is wasted, but it's more
       convenient to say tags[OPT_SONG] than to enumerate all the
       metadata types (again) as a struct. */
    const char *tags[UCHAR_MAX];
    int nums[UCHAR_MAX];

    memset( tags, 0, sizeof( tags ) );
    memset( nums, 0, sizeof( nums ) );

    /* Any modifications requested? */
    int mods = 0;

    /* Option-processing loop. */
    int c = prog::getOptionSingle( argc, argv, OPT_STRING, long_options, NULL );
    while ( c != -1 ) {
        int r = 2;
        switch ( c ) {
                /* getopt() returns '?' if there was an error.  It already
                   printed the error message, so just return. */
            case '?':
                return 1;

                /* Help and version requests handled here. */
            case OPT_HELP:
                fprintf( stderr, "usage %s %s\n", argv[0], help_text );
                return 0;
            case OPT_VERSION:
                fprintf( stderr, "%s - %s\n", argv[0], MP4V2_PROJECT_name_formal );
                return 0;

                /* Numeric arguments: convert them using sscanf(). */
            case OPT_DISK:
            case OPT_DISKS:
            case OPT_TRACK:
            case OPT_TRACKS:
            case OPT_HD:
            case OPT_CNID:
            case OPT_TVEPISODE:
            case OPT_TEMPO:
                r = sscanf( prog::optarg, "%d", &nums[c] );
                if ( r < 1 ) {
                    fprintf( stderr, "%s: option requires numeric argument -- %c\n",
                             argv[0], c );
                    return 2;
                }
                /* Break not, lest ye be broken.  :) */
                /* All arguments: all valid options end up here, and we just
                   stuff the string pointer into the tags[] array. */
            default:
                tags[c] = prog::optarg;
                mods++;
        } /* end switch */

        c = prog::getOptionSingle( argc, argv, OPT_STRING, long_options, NULL );
    } /* end while */

    /* Check that we have at least one non-option argument */
    if ( ( argc - prog::optind ) < 1 ) {
        fprintf( stderr,
                 "%s: You must specify at least one MP4 file.\n",
                 argv[0] );
        fprintf( stderr, "usage %s %s\n", argv[0], help_text );
        return 3;
    }

    /* Check that we have at least one requested modification.  Probably
       it's useful instead to print the metadata if no modifications are
       requested? */
    if ( !mods ) {
        fprintf( stderr,
                 "%s: You must specify at least one tag modification.\n",
                 argv[0] );
        fprintf( stderr, "usage %s %s\n", argv[0], help_text );
        return 4;
    }

    /* Loop through the non-option arguments, and modify the tags as
       requested. */
    while ( prog::optind < argc ) {
        char *mp4 = argv[prog::optind++];

        MP4FileHandle h = MP4Modify( mp4 );
        if ( h == MP4_INVALID_FILE_HANDLE ) {
            fprintf( stderr, "Could not open '%s'... aborting\n", mp4 );
            return 5;
        }

        /* Remove any tags */
        if ( ELEMENT_OF(tags,OPT_REMOVE) ) {
            for ( const char *p = ELEMENT_OF(tags,OPT_REMOVE); *p; p++ ) {
                switch ( *p ) {
                    case OPT_ALBUM:
                        MP4DeleteMetadataAlbum( h );
                        break;
                    case OPT_ARTIST:
                        MP4DeleteMetadataArtist( h );
                        break;
                    case OPT_COMMENT:
                        MP4DeleteMetadataComment( h );
                        break;
                    case OPT_COPYRIGHT:
                        MP4DeleteMetadataCopyright( h );
                        break;
                    case OPT_DISK:
                        MP4DeleteMetadataDisk( h );
                        break;
                    case OPT_DISKS:
                        MP4DeleteMetadataDisk( h );
                        break;
                    case OPT_ENCODEDBY:
                        MP4DeleteMetadataEncodedBy( h );
                        break;
                    case OPT_TOOL:
                        MP4DeleteMetadataTool( h );
                        break;
                    case OPT_GENRE:
                        MP4DeleteMetadataGenre( h );
                        break;
                    case OPT_GROUPING:
                        MP4DeleteMetadataGrouping( h );
                        break;
                    case OPT_HD:
                        MP4DeleteMetadataHDVideo( h );
                        break;
                    case OPT_CNID:
                        MP4DeleteMetadataCNID( h );
                        break;
                    case OPT_MEDIA_TYPE:
                        MP4DeleteMetadataMediaType( h );
                        break;
                    case OPT_DESCRIPTION:
                        MP4DeleteMetadataShortDescription( h );
                        break;
                    case OPT_TVEPISODE:
                        MP4DeleteMetadataTVEpisode( h );
                        break;
                    case OPT_SONG:
                        MP4DeleteMetadataName( h );
                        break;
                    case OPT_COMPOSER:
                        MP4DeleteMetadataComposer( h );
                        break;
                    case OPT_YEAR:
                        MP4DeleteMetadataReleaseDate( h );
                        break;
                    case OPT_TEMPO:
                        MP4DeleteMetadataBPM( h );
                        break;
                    case OPT_TRACK:
                        MP4DeleteMetadataTrack( h );
                        break;
                    case OPT_TRACKS:
                        MP4DeleteMetadataTrack( h );
                        break;
                    case OPT_PICTURE:
                        MP4DeleteMetadataCoverArt( h );
                        break;
                    case OPT_ALBUM_ARTIST:
                        MP4DeleteMetadataAlbumArtist( h );
                        break ;
                }
            }
        }

        /* Track/disk numbers need to be set all at once, but we'd like to
           allow users to just specify -T 12 to indicate that all existing
           track numbers are out of 12.  This means we need to look up the
           current info if it is not being set. */
        uint16_t n0, m0, n1, m1;

        if ( ELEMENT_OF(tags,OPT_TRACK) || ELEMENT_OF(tags,OPT_TRACKS) ) {
            n0 = m0 = 0;
            MP4GetMetadataTrack( h, &n0, &m0 );
            n1 = ELEMENT_OF(tags,OPT_TRACK) ? ELEMENT_OF(nums,OPT_TRACK) : n0;
            m1 = ELEMENT_OF(tags,OPT_TRACKS) ? ELEMENT_OF(nums,OPT_TRACKS) : m0;
            MP4SetMetadataTrack( h, n1, m1 );
        }
        if ( ELEMENT_OF(tags,OPT_DISK) || ELEMENT_OF(tags,OPT_DISKS) ) {
            n0 = m0 = 0;
            MP4GetMetadataDisk( h, &n0, &m0 );
            n1 = ELEMENT_OF(tags,OPT_DISK) ? ELEMENT_OF(nums,OPT_DISK) : n0;
            m1 = ELEMENT_OF(tags,OPT_DISKS) ? ELEMENT_OF(nums,OPT_DISKS) : m0;
            MP4SetMetadataDisk( h, n1, m1 );
        }

        /* Set the other relevant attributes */
        for ( int i = 0;  i < UCHAR_MAX;  i++ ) {
            if ( tags[i] ) {
                switch ( i ) {
                    case OPT_ALBUM:
                        MP4SetMetadataAlbum( h, tags[i] );
                        break;
                    case OPT_ARTIST:
                        MP4SetMetadataArtist( h, tags[i] );
                        break;
                    case OPT_COMMENT:
                        MP4SetMetadataComment( h, tags[i] );
                        break;
                    case OPT_COPYRIGHT:
                        MP4SetMetadataCopyright( h, tags[i] );
                        break;
                    case OPT_ENCODEDBY:
                        MP4SetMetadataEncodedBy( h, tags[i] );
                        break;
                    case OPT_TOOL:
                        MP4SetMetadataTool( h, tags[i] );
                        break;
                    case OPT_GENRE:
                        MP4SetMetadataGenre( h, tags[i] );
                        break;
                    case OPT_GROUPING:
                        MP4SetMetadataGrouping( h, tags[i] );
                        break;
                    case OPT_HD:
                        MP4SetMetadataHDVideo( h, nums[i] );
                        break;
                    case OPT_CNID:
                        MP4SetMetadataCNID( h, nums[i] );
                        break;
                    case OPT_MEDIA_TYPE:
                    {
                        itmf::StikType st = itmf::enumStikType.toType( tags[i] );
                        MP4SetMetadataMediaType( h, st );
                        break;
                    }
                    case OPT_DESCRIPTION:
                        MP4SetMetadataShortDescription( h, tags[i] );
                        break;
                    case OPT_TVEPISODE:
                        MP4SetMetadataTVEpisode( h, nums[i] );
                        break;
                    case OPT_SONG:
                        MP4SetMetadataName( h, tags[i] );
                        break;
                    case OPT_COMPOSER:
                        MP4SetMetadataComposer( h, tags[i] );
                        break;
                    case OPT_YEAR:
                        MP4SetMetadataReleaseDate( h, tags[i] );
                        break;
                    case OPT_TEMPO:
                        MP4SetMetadataBPM( h, nums[i] );
                        break;
                    case OPT_ALBUM_ARTIST:
                        MP4SetMetadataAlbumArtist( h, tags[i] );
                        break;
                    case OPT_PICTURE:
                    {
                        // TODO-KB: test io::StdioFile
                        io::StdioFile out( tags[i] );
                        if( !out.open( "rb" )) {
                            io::StdioFile::Size artSize;
                            out.getSize( artSize );

                            uint8_t* art = (uint8_t*)malloc( (size_t)artSize );

                            io::StdioFile::Size nin;
                            if( out.read( art, artSize, nin ) && nin == artSize ) {
                                MP4SetMetadataCoverArt( h, art, artSize );
                            }

                            free( art );
                            out.close();
                        }
                        else {
                            fprintf( stderr, "Art file %s not found\n", tags[i] );
                        }
                    }
                }
            }
        }

        MP4Close( h );
    } /* end while optind < argc */
    return 0;
}