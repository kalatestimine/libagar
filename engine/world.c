/*	$Csoft: world.c,v 1.28 2002/05/03 20:13:19 vedge Exp $	*/

/*
 * Copyright (c) 2001, 2002 CubeSoft Communications, Inc.
 * <http://www.csoft.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistribution of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistribution in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of CubeSoft Communications, nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <errno.h>

#include <engine/engine.h>
#include <engine/map.h>
#include <engine/physics.h>

#include <engine/mapedit/mapedit.h>

static const struct obvec world_vec = {
	NULL,
	world_load,
	world_save,
	NULL,		/* link */
	NULL		/* unlink */
};

char *
savepath(char *obname, const char *suffix)
{
	static char path[FILENAME_MAX];
	static struct stat sta;
	char *p, *last;
	char *datapath, *datapathp;
	
	datapathp = datapath = strdup(world->datapath);

	for (p = strtok_r(datapath, ":;", &last);
	     p != NULL;
	     p = strtok_r(NULL, ":;", &last)) {
		sprintf(path, "%s/%s.%s", p, obname, suffix);
		if (stat(path, &sta) == 0) {
			free(datapathp);
			return (path);
		}
	}

	fatal("%s.%s not in %s\n", obname, suffix, world->datapath);
	free(datapathp);
	return (NULL);
}

void
world_init(struct world *wo, char *name)
{
	struct passwd *pwd;
	struct stat sta;
	char *pathenv;
	
	pwd = getpwuid(getuid());

	object_init(&wo->obj, name, NULL, 0, &world_vec);

	wo->udatadir = (char *)emalloc(strlen(pwd->pw_dir) + strlen(name) + 4);
	wo->sysdatadir = (char *)emalloc(strlen(SHAREDIR) + strlen(name) + 4);
	sprintf(wo->udatadir, "%s/.%s", pwd->pw_dir, name);
	sprintf(wo->sysdatadir, SHAREDIR);

	pathenv = getenv("AGAR_STATE_PATH");
	if (pathenv != NULL) {
		wo->datapath = strdup(pathenv);
	} else {
		wo->datapath = (char *)
		    emalloc(strlen(wo->udatadir) + strlen(wo->sysdatadir) + 4);
		sprintf(wo->datapath, "%s:%s", wo->udatadir, wo->sysdatadir);
	}

	if (stat(wo->sysdatadir, &sta) != 0) {
		warning("%s: %s\n", wo->sysdatadir, strerror(errno));
	}
	if (stat(wo->udatadir, &sta) != 0 &&
	    mkdir(wo->udatadir, 00700) != 0) {
		fatal("%s: %s\n", wo->udatadir, strerror(errno));
	}

	wo->curmap = NULL;
	wo->nobjs = 0;
	wo->nchars = 0;

	SLIST_INIT(&wo->wobjsh);
	SLIST_INIT(&wo->wcharsh);
	pthread_mutex_init(&wo->lock, NULL);
}

/*
 * Load state of active objects.
 */
int
world_load(void *p, int fd)
{
	struct world *wo = (struct world *)p;
	struct object *ob;

	/* XXX load the state map */

	pthread_mutex_assert(&wo->lock);
	SLIST_FOREACH(ob, &world->wobjsh, wobjs) {
		dprintf("loading %s\n", ob->name);

		if (curmapedit != NULL && !strcmp(ob->saveext, "m")) {
			/* XXX map editor hack */
			continue;
		}
		object_load(ob);
	}
	printf("%s: loaded %d objects\n", OBJECT(wo)->name, wo->nobjs);
	return (0);
}

/*
 * Save the world!
 */
int
world_save(void *p, int fd)
{
	struct world *wo = (struct world *)p;
	struct object *ob;

	pthread_mutex_assert(&wo->lock);
	SLIST_FOREACH(ob, &world->wobjsh, wobjs) {
		dprintf("saving %s\n", ob->name);

		if (curmapedit != NULL && !strcmp(ob->saveext, "m")) {
			/* XXX map editor hack */
			continue;
		}
		fobj_write_uint32(fd, ob->id);
		fobj_write_string(fd, ob->name);
		fobj_write_string(fd, (ob->desc != NULL) ? ob->desc : "");
		object_save(ob);
	}

	printf("%s: saved %d objects\n", OBJECT(wo)->name, wo->nobjs);
	return (0);
}

/*
 * Destroy the world!
 */
void
world_destroy(void *p)
{
	struct world *wo = (struct world *)p;
	struct object *ob, *nextob;

	if (world->curmap != NULL) {
		map_unfocus(world->curmap);
	}

	printf("freed:");
	fflush(stdout);
	pthread_mutex_lock(&wo->lock);
	for (ob = SLIST_FIRST(&wo->wobjsh);
	     ob != SLIST_END(&wo->wobjsh);
	     ob = nextob) {
		nextob = SLIST_NEXT(ob, wobjs);
		printf(" %s", ob->name);
		fflush(stdout);
		object_queue_gc(ob);
		free(ob);
	}
	pthread_mutex_unlock(&wo->lock);
	printf(".\n");

	free(wo->datapath);
	free(wo->udatadir);
	free(wo->sysdatadir);
	pthread_mutex_destroy(&wo->lock);
}

