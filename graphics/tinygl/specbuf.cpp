
#include "graphics/tinygl/zgl.h"

namespace TinyGL {

static void calc_buf(GLSpecBuf *buf, const float shininess) {
	float val, inc;
	val = 0.0f;
	inc = 1.0f / SPECULAR_BUFFER_SIZE;
	for (int i = 0; i <= SPECULAR_BUFFER_SIZE; i++) {
		buf->buf[i] = pow(val, shininess);
		val += inc;
	}
}

GLSpecBuf *specbuf_get_buffer(GLContext *c, const int shininess_i, const float shininess) {
	GLSpecBuf *found, *oldest;
	found = oldest = c->specbuf_first;
	while (found && found->shininess_i != shininess_i) {
		if (found->last_used < oldest->last_used) {
			oldest = found;
		}
		found = found->next; 
	}
	if (found) {
		found->last_used = c->specbuf_used_counter++;
		return found;
	}
	if (!oldest || c->specbuf_num_buffers < MAX_SPECULAR_BUFFERS) {
		// create new buffer
		GLSpecBuf *buf = (GLSpecBuf *)gl_malloc(sizeof(GLSpecBuf));
		if (!buf)
			error("could not allocate specular buffer");
		c->specbuf_num_buffers++;
		buf->next = c->specbuf_first;
		c->specbuf_first = buf;
		buf->last_used = c->specbuf_used_counter++;
		buf->shininess_i = shininess_i;
		calc_buf(buf, shininess);
		return buf;     
	}
	//overwrite the lru buffer
	oldest->shininess_i = shininess_i;
	oldest->last_used = c->specbuf_used_counter++;
	calc_buf(oldest, shininess);
	return oldest;
}

void specbuf_cleanup(GLContext *c) {
	GLSpecBuf *buf, *next;
	buf = c->specbuf_first;
	for (int i = 0; i < c->specbuf_num_buffers; ++i) {
		next = buf->next;
		gl_free(buf);
		buf = next;
	}
}

} // end of namespace TinyGL
