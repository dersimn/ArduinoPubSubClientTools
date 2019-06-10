#include "MqttWildcard.h"

int MqttWildcard::explode(String *results, String source, char delimiter) {
    int count = 0;
    int index = 0;
    
    for (int i = 0; i < source.length(); i++) {
        if (source.charAt(i) == delimiter) {
            results[count++] = source.substring(index, i);
            index = i+1;
        }
    }
    results[count++] = source.substring(index);

    return count;
}
bool MqttWildcard::wildcardMatch(String topic, String wildcard) {
    // Catch trivial matches
    if (topic == wildcard) return true;
    if (wildcard == "#") return true;

    String exploded_topic[TOPIC_BUFFER_SIZE];
    int exploded_topic_count = MqttWildcard::explode(exploded_topic, topic, '/');

    String exploded_wildcard[TOPIC_BUFFER_SIZE];
    int exploded_wildcard_count = MqttWildcard::explode(exploded_wildcard, wildcard, '/');

    // Impossible to match since wildcard "+/+/#" is not matched by topic foo/bar
    if (exploded_wildcard_count > exploded_topic_count) return false;

    int match_count = 0;
    for (int i = 0; i < exploded_wildcard_count; i++) {
        if (exploded_wildcard[i] == "+") {
            continue;
        }
        if (exploded_wildcard[i] == "#") {
            return true;
        }
        if (exploded_wildcard[i] != exploded_topic[i]) {
            return false;
        }
    }

    /*
        If this point is reached and we did not return yet,
        topic- and wildcard-depth must be equal, otherwise it cant't be a valid match:
            topic:          foo/bar/example
            wildcard_1:     foo/bar/+
            wildcard_2:     foo/+
        Both wildcards would make it to this point, bot only wildcard_1 would be a valid match.
    */
    return (exploded_wildcard_count == exploded_topic_count);
}