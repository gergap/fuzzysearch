#include <string.h>
#include "metric.h"

/** Computes the string metric for the given strings.
 *
 * The algorithm used is from this site:
 * http://www.catalysoft.com/articles/strikeamatch.html
 *
 * a: first string
 * b: second string
 * f(x): returns the set of adjacent letter pairs of string x.
 *       x="string" -> f(x) = {"st", "tr", "ri", "in", "ng" }
 * A = f(a)
 * B = f(b)
 * Metric = 2 * |f(a) \cap f(b)| / ( |f(a)| + |f(b)| )
 * This way Metric is in the range of 0..1.
 * string: st tr ri in ng
 * strong: st tr ro on ng
 * result: 3 of 5 identical ( "st", "tr", "ng" )
 *
 * This implementation avoids creating lists of letter pairs to compute the metric.
 * Instead it iterates just over the strings and compares the letter pairs in place.
 *
 * @param data data to compare for query
 * @param query query string
 *
 * @return a value between 0..1000 indicating the likelihood of a match.
 * or -1 in case of an error.
 */
int string_metric(const char *data, const char *query)
{
    int lendata = strlen(data);
    int lenquery = strlen(query);
    int i = 0;
    int j = 0;
    char a1, a2;
    char b1, b2;
    int n = lendata + lenquery - 2; /* |f(a)| + |f(b)| */
    int m = 0; /* matches */

    /* sanity check: both strings must be at least two letters long */
    if (lendata < 2 || lenquery < 2) return -1;

    a2 = query[i++];
    while (query[i] != 0) {
        a1 = a2;
        a2 = query[i++];
        j = 0;
        b2 = data[j++];
        while (data[j] != 0) {
            b1 = b2;
            b2 = data[j++];
            if (a1 == b1 && a2 == b2) {
                m++;
                break; /* count each occurance of pair in data only one time */
            }
        }
    }

    /* compute scaled result. */
    m = 2000 * m / n;
    return m;
}

