#include "config.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

char* trim(char* str) {
  while (*str && isspace((unsigned char) *str)) {
    str++;
  }
  char* end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char) *end)) {
    *end-- = '\0';
  }
  return str;
}

int extract_array_config_line(config_line_t* config_line, char* data) {
  EXTRACT_ARRAY(config_line, data);
  return EXIT_SUCCESS;
}

int parse_line(config_line_vec_t* config_line_vec, char* line) {
  config_line_t config_line;
  config_line.scope = SCOPE_NONE;

  if (str_vec_new(&config_line.fields) != 0) {
    return EXIT_FAILURE;
  }
  if (str_vec_new(&config_line.array) != 0) {
    str_vec_free(&config_line.fields);
    return EXIT_FAILURE;
  }

  bool  scope_extracted = false;
  char* saveptr;
  char* data = strtok_r(line, ";", &saveptr);

  while (data != NULL) {
    data = trim(data);
    if (*data == '\0') {
      LOG_ERROR("Empty field in the config file.");
      str_vec_free(&config_line.fields);
      str_vec_free(&config_line.array);
      return EXIT_FAILURE;
    }

    if (!scope_extracted) {
      config_line.scope = scope_str_to_enum(data);
      if (config_line.scope == SCOPE_UNKNOWN) {
        LOG_ERROR("Unknown scope name!");
        str_vec_free(&config_line.fields);
        str_vec_free(&config_line.array);
        return EXIT_FAILURE;
      }
      scope_extracted = true;
      data            = strtok_r(NULL, ";", &saveptr);
      continue;
    }

    if ((strchr(data, ',')) != NULL) {
      if (extract_array_config_line(&config_line, data)) {
        LOG_ERROR("Failed to extract array");
        str_vec_free(&config_line.fields);
        str_vec_free(&config_line.array);
        return EXIT_FAILURE;
      }
      data = strtok_r(NULL, ";", &saveptr);
      continue;
    }

    str_vec_push(config_line.fields, data);
    data = strtok_r(NULL, ";", &saveptr);
  }

  if (config_line_vec_push(config_line_vec, config_line) != 0) {
    str_vec_free(&config_line.fields);
    str_vec_free(&config_line.array);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int parse_file(const char* filename, config_line_vec_t** config_line_vec) {
  if (config_line_vec_new(config_line_vec) != 0) {
    LOG_ERROR("Failed to create config line vector");
    return EXIT_FAILURE;
  }

  FILE* file_p = fopen(filename, "rb");
  if (!file_p) {
    LOG_ERROR("Failed to open file");
    return EXIT_FAILURE;
  }

  fseek(file_p, (size_t) 0, SEEK_END);
  size_t file_size = ftell(file_p);
  fseek(file_p, (size_t) 0, SEEK_SET);

  char* buffer = malloc(file_size + 1);
  if (!buffer) {
    LOG_ERROR("Failed to allocate buffer");
    fclose(file_p);
    return EXIT_FAILURE;
  }

  size_t read  = fread(buffer, (size_t) 1, file_size, file_p);
  buffer[read] = '\0';
  fclose(file_p);

  if (read != file_size) {
    LOG_ERROR("Failed to read file");
    free(buffer);
    return EXIT_FAILURE;
  }

  char* saveptr;
  char* token = strtok_r(buffer, "\n", &saveptr);

  while (token != NULL) {
    if (*token == '\0') {
      token = strtok_r(NULL, "\n", &saveptr);
      continue;
    }

    token = trim(token);

    if (parse_line((*config_line_vec), token)) {
      LOG_ERROR("Failed to parse line");

      for (size_t i = 0; i < (*config_line_vec)->len; i++) {
        config_line_t* line = &(*config_line_vec)->data[i];
        str_vec_free(&line->fields);
        str_vec_free(&line->array);
      }
      config_line_vec_free(config_line_vec);

      free(buffer);
      return EXIT_FAILURE;
    }

    token = strtok_r(NULL, "\n", &saveptr);
  }

  free(buffer);

  return EXIT_SUCCESS;
}

int sort_by_enums(config_line_vec_t* config_line_vec) {
  for (size_t i = 0; i < config_line_vec->len - 1; i++) {
    for (size_t j = i; j < config_line_vec->len - i - 2; j++) {
      if (config_line_vec->data[j].scope > config_line_vec->data[j + 1].scope) {
        config_line_t tmp            = config_line_vec->data[j];
        config_line_vec->data[j]     = config_line_vec->data[j + 1];
        config_line_vec->data[j + 1] = tmp;
      }
    }
  }
  return EXIT_SUCCESS;
}

int sort_by_fields(config_line_vec_t* config_line_vec) {
  size_t split = 0;

  for (; split < config_line_vec->len; split++) {
    if (config_line_vec->data[split].scope == 2) {
      break;
    }
  }

  for (size_t i = 0; i < split; i++) {
    for (size_t j = 0; j < split - i - 1; j++) {
      if (strcmp(config_line_vec->data[j].fields->str[0],
                 config_line_vec->data[j + 1].fields->str[0])
          > 0) {
        config_line_t tmp            = config_line_vec->data[j];
        config_line_vec->data[j]     = config_line_vec->data[j + 1];
        config_line_vec->data[j + 1] = tmp;
      }
    }
  }

  for (size_t i = split + 1; i < config_line_vec->len - 1; i++) {
    for (size_t j = split + 1; j < config_line_vec->len - (i - split) - 1; j++) {
      if (strcmp(config_line_vec->data[j].fields->str[0],
                 config_line_vec->data[j + 1].fields->str[0])
          > 0) {
        config_line_t tmp            = config_line_vec->data[j];
        config_line_vec->data[j]     = config_line_vec->data[j + 1];
        config_line_vec->data[j + 1] = tmp;
      }
    }
  }

  return EXIT_SUCCESS;
}

int write_config(config_line_vec_t* config_line_vec, const char* filename) {
  if (sort_by_enums(config_line_vec)) {
    LOG_ERROR("sort_by_enums failed!");
    return EXIT_FAILURE;
  }

  if (sort_by_fields(config_line_vec)) {
    LOG_ERROR("sort_by_fields failed!");
    return EXIT_FAILURE;
  }

  FILE* file_p = fopen(filename, "w");
  if (!file_p) {
    LOG_ERROR("Failed to open file for writing");
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < config_line_vec->len; i++) {
    fprintf(file_p, ":%s", scope_enum_to_str(config_line_vec->data[i].scope));
    for (size_t j = 0; j < config_line_vec->data[j].fields->len; j++) {
      fprintf(file_p, ":%s", config_line_vec->data[i].fields->str[j]);
    }
    if (config_line_vec->data[i].array != NULL
        && config_line_vec->data[i].array->len > 0) {
      fprintf(file_p, ":");
      for (size_t k = 0; k < config_line_vec->data->array->len; k++) {
        fprintf(file_p, "%s", config_line_vec->data->array->str[k]);
        if (k < config_line_vec->data[i].array->len - 1) {
          fprintf(file_p, ",");
        }
      }
      fprintf(file_p, ";");
    } else {
      fprintf(file_p, ";");
    }
    fprintf(file_p, "\n");
  }

  fclose(file_p);
  return EXIT_SUCCESS;
}
