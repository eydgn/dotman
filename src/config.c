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

int extract_array(config_line_t* config_line, char* data) {
  char* saveptr;
  char* element = strtok_r(data, ",", &saveptr);
  while (element != NULL) {
    element = trim(element);
    if (*element == '\0') {
      LOG_ERROR("Empty element in the array field.");
      return EXIT_FAILURE;
    }
    if (str_vec_push(config_line->array, element)) {
      LOG_ERROR("Failed to push element to array.");
      return EXIT_FAILURE;
    }
    element = strtok_r(NULL, ",", &saveptr);
  }
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
      if (extract_array(&config_line, data)) {
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
  for (size_t i = 0; i < config_line_vec->len; i++) {
    for (size_t j = i; j < config_line_vec->len - 1; j++) {
      if (config_line_vec[j].data->scope > config_line_vec[j + 1].data->scope) {
        config_line_vec_t tmp  = config_line_vec[j];
        config_line_vec[j]     = config_line_vec[j + 1];
        config_line_vec[j + 1] = tmp;
      }
    }
  }
  return EXIT_SUCCESS;
}

int sort_by_fields(config_line_vec_t* config_line_vec) {
  size_t split = 0;

  for (; split < config_line_vec->len; split++) {
    if (config_line_vec[split].data->scope == 2) {
      break;
    }
  }

  for (size_t i = 0; i < split - 1; i++) {
    for (size_t j = i; j < split - 1; j++) {
      if (strcmp(config_line_vec[j].data->fields->str[0],
                 config_line_vec[j + 1].data->fields->str[0])
          > 0) {
        config_line_vec_t tmp  = config_line_vec[j];
        config_line_vec[j]     = config_line_vec[j + 1];
        config_line_vec[j + 1] = tmp;
      }
    }
  }

  for (size_t i = split + 1; i < config_line_vec->len - 2; i++) {
    for (size_t j = i; j < config_line_vec->len - 2; j++) {
      if (strcmp(config_line_vec[j].data->fields->str[0],
                 config_line_vec[j + 1].data->fields->str[0])
          > 0) {
        config_line_vec_t tmp  = config_line_vec[j];
        config_line_vec[j]     = config_line_vec[j + 1];
        config_line_vec[j + 1] = tmp;
      }
    }
  }

  return EXIT_SUCCESS;
}
